//
// Created by rgnter on 25/11/2024.
//

#include "server/DataDirector.hpp"

#include <spdlog/spdlog.h>

namespace
{

//!
template<typename T>
void ProvideLockedDatumAccess(
  alicia::DataDirector::Datum<T>& datum,
  const alicia::DatumConsumer<const T&>& immutableConsumer)
{
  std::scoped_lock lock(datum.lock);
  try
  {
    immutableConsumer(datum.value);
  }
  catch (const std::exception& x)
  {
    spdlog::error("Unhandled exception in immutable datum consumer:", x);
  }
}

//!
template<typename T>
void ProvideLockedMutableDatumAccess(
  alicia::DataDirector::Datum<T>& datum,
  alicia::DatumConsumer<T&>& mutableConsumer)
{
  std::scoped_lock lock(datum.lock);
  try
  {
    mutableConsumer(datum.value);
  }
  catch (const std::exception& x)
  {
    spdlog::error("Unhandled exception in mutable datum consumer:", x);
  }
}

} // anon namespace

namespace alicia
{

void DataDirector::GetUser(
  DatumUid userUid,
  DatumConsumer<const UserCharacter&> consumer)
{
  const auto userItr = _characters.find(userUid);
  if (userItr == _characters.cend())
  {
    throw std::runtime_error("User doesn't exist");
  }

  ProvideLockedDatumAccess(userItr->second, consumer);
}

void DataDirector::GetUserMutable(
  DatumUid userUid,
  DatumConsumer<UserCharacter&> consumer)
{
  const auto userItr = _characters.find(userUid);
  if (userItr == _characters.cend())
  {
    return;
  }

  ProvideLockedMutableDatumAccess(userItr->second, consumer);
}

void DataDirector::GetMount(
  DatumUid mountUid,
  DatumConsumer<const UserMount&> consumer)
{
  ProvideLockedDatumAccess(_mounts[mountUid], consumer);
}

void DataDirector::GetMountMutable(
  DatumUid mountUid,
  DatumConsumer<UserMount&> consumer)
{
  ProvideLockedMutableDatumAccess(_mounts[mountUid], consumer);
}

void DataDirector::GetRanch(
  DatumUid ranchUid,
  DatumConsumer<const UserRanch&> consumer)
{
  ProvideLockedDatumAccess(_ranches[ranchUid], consumer);
}

void DataDirector::GetRanchMutable(
  DatumUid ranchUid,
  DatumConsumer<UserRanch&> consumer)
{
  ProvideLockedMutableDatumAccess(_ranches[ranchUid], consumer);
}

} // namespace alicia
