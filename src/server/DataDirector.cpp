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
  const std::string& name,
  DatumConsumer<const User&> consumer)
{
  consumer(_users[name].value);
}

void DataDirector::GetUserMutable(
  const std::string& name,
  DatumConsumer<User&> consumer)
{
  throw std::runtime_error("not implemented");
}

void DataDirector::GetCharacter(
  DatumUid characterUid,
  DatumConsumer<const User::Character&> consumer)
{
  ProvideLockedDatumAccess(_characters[characterUid], consumer);
}

void DataDirector::GetCharacterMutable(
  DatumUid characterUid,
  DatumConsumer<User::Character&> consumer)
{
  ProvideLockedMutableDatumAccess(_characters[characterUid], consumer);
}

void DataDirector::GetMount(
  DatumUid mountUid,
  DatumConsumer<const User::Mount&> consumer)
{
  ProvideLockedDatumAccess(_mounts[mountUid], consumer);
}

void DataDirector::GetMountMutable(
  DatumUid mountUid,
  DatumConsumer<User::Mount&> consumer)
{
  throw std::runtime_error("not implemented");
}

void DataDirector::GetRanch(
  DatumUid ranchUid,
  DatumConsumer<const User::Ranch&> consumer)
{
  ProvideLockedDatumAccess(_ranches[ranchUid], consumer);
}

void DataDirector::GetRanchMutable(
  DatumUid mountUid,
  DatumConsumer<User::Ranch&> consumer)
{
  throw std::runtime_error("not implemented");
}

} // namespace alicia
