//
// Created by alborrajo on 16/11/2024.
//

#ifndef DIRECTOR_HPP
#define DIRECTOR_HPP

#include <libserver/command/CommandServer.hpp>

namespace alicia {

//! Unique user identifier.
using UserId = std::uint32_t;
//! Invalid user identifier.
constexpr UserId InvalidUserId = std::numeric_limits<UserId>::max();

using HorseId = std::uint32_t;

//! Unique ranch identifier.
using RanchId = std::uint32_t;

struct HorseData
{
  uint32_t tid;
  std::string name;
};

//! User.
struct User
{
  std::string nickName;
  Gender gender = Gender::Unspecified;

  uint16_t level{};
  int32_t carrots{};

  std::vector<Item> characterEquipment;

  HorseId mountUid{};
  std::vector<HorseId> horses{};

  std::chrono::system_clock::time_point lastHeartbeat;

  RanchId ranchUid{};
};

struct Ranch
{
  std::string ranchName;
  std::vector<HorseId> horses{};
  std::vector<UserId> users{};
};

}

#endif //DIRECTOR_HPP