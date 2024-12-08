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
  const alicia::DatumConsumer<T&>& immutableConsumer)
{
  std::scoped_lock lock(datum.lock);
  try
  {
    immutableConsumer(datum.value);
  }
  catch (const std::exception& x)
  {
    spdlog::error("Unhandled exception in immutable datum consumer:", x.what());
  }
}


} // anon namespace

namespace alicia
{

void DataDirector::GetUser(
  const std::string& name,
  DatumConsumer<User&> consumer)
{
  consumer(_users[name].value);
}

void DataDirector::GetCharacter(
  DatumUid characterUid,
  DatumConsumer<User::Character&> consumer)
{
  ProvideLockedDatumAccess(_characters[characterUid], consumer);
}



void DataDirector::GetMount(
  DatumUid mountUid,
  DatumConsumer<User::Mount&> consumer)
{
  ProvideLockedDatumAccess(_mounts[mountUid], consumer);
}

void DataDirector::GetRanch(
  DatumUid ranchUid,
  DatumConsumer<User::Ranch&> consumer)
{
  ProvideLockedDatumAccess(_ranches[ranchUid], consumer);
}


} // namespace alicia
