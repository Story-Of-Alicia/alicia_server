//
// Created by rgnter on 10/11/2024.
//

#ifndef LOBBYDIRECTOR_HPP
#define LOBBYDIRECTOR_HPP

#include <libserver/command/CommandServer.hpp>

namespace alicia
{

//! Unique user identifier.
using UserId = std::uint32_t;
//! Invalid user identifier.
constexpr UserId InvalidUserId = std::numeric_limits<UserId>::max();

//! User.
struct User
{
  std::string nickName;
  Gender gender = Gender::Unspecified;

  uint16_t level{};
  int32_t carrots{};

  std::chrono::system_clock::time_point lastHeartbeat;
};

class LoginDirector
{
public:
  explicit LoginDirector(CommandServer& lobbyServer) noexcept;

  //!
  void HandleUserLogin(
    ClientId clientId,
    const LobbyCommandLogin& login);

  //!
  void HandleHeartbeat(
    ClientId clientId,
    const LobbyCommandHeartbeat& heartbeat);

  //!
  void HandleShowInventory(
    ClientId clientId,
    const LobbyCommandShowInventory& showInventory);

  //!
  void HandleAchievementCompleteList(
    ClientId clientId,
    const LobbyCommandAchievementCompleteList& achievementCompleteList);

  //!
  void HandleRequestLeagueInfo(
    ClientId clientId,
    const LobbyCommandRequestLeagueInfo& requestLeagueInfo);

  //!
  void HandleRequestQuestList(
    ClientId clientId, const
    LobbyCommandRequestQuestList& requestQuestList);

private:
  CommandServer& _lobbyServer;

  std::unordered_map<UserId, std::string> _userTokens;
  std::unordered_map<UserId, ClientId> _clients;

  std::unordered_map<UserId, User> _users;
};

}

#endif //LOBBYDIRECTOR_HPP
