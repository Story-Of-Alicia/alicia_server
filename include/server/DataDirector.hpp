//
// Created by rgnter on 25/11/2024.
//

#ifndef DATADIRECTOR_HPP
#define DATADIRECTOR_HPP

#include <chrono>
#include <cstdint>
#include <functional>
#include <mutex>
#include <unordered_map>

#include <libserver/command/proto/DataDefines.hpp>

namespace alicia
{

//! Unique datum identifier.
using DatumUid = uint32_t;
//! Invalid datum identifier.
constexpr DatumUid InvalidDatumUid = 0;

//! Datum consumer is a callback which provides access to a value,
//! to which access is guaranteed while in the consumer function.
template<typename T>
using DatumConsumer = std::function<void(T value)>;

//! User
struct User
{
  std::string nickName;
  Gender gender = Gender::Unspecified;
  uint16_t level{};
  int32_t carrots{};

  std::vector<Item> characterEquipment;
  std::vector<Item> horseEquipment;

  DatumUid mountUid{};
  DatumUid ranchUid{};

  std::chrono::system_clock::time_point lastHeartbeat;
};

//! Mount
struct Mount
{
  uint32_t tid{};
  std::string name;
};

//! Ranch
struct Ranch
{
  std::string ranchName;
  std::vector<DatumUid> horses;
  std::vector<DatumUid> users;
};

class DataDirector
{
public:
  //!
  void GetUser(
    DatumUid userUid,
    DatumConsumer<const User&> consumer);

  //!
  void GetUserMutable(
    DatumUid userUid,
    DatumConsumer<User&> consumer);

  //!
  void GetMount(
    DatumUid mountUid,
    DatumConsumer<const Mount&> consumer);
  //!
  void GetMountMutable(
    DatumUid mountUid,
    DatumConsumer<Mount&> consumer);

  //!
  void GetRanch(
    DatumUid ranchUid,
    DatumConsumer<const Ranch&> consumer);
  //!
  void GetRanchMutable(
    DatumUid mountUid,
    DatumConsumer<Ranch&> consumer);

private:
  //!
  template<typename Val>
  struct Datum
  {
    Val value;
    std::mutex lock;
  };

  //!
  std::unordered_map<DatumUid, Datum<User>> _users;
  //!
  std::unordered_map<DatumUid, Datum<Mount>> _mounts;
  //!
  std::unordered_map<DatumUid, Datum<Ranch>> _ranches;
};

}

#endif //DATADIRECTOR_HPP
