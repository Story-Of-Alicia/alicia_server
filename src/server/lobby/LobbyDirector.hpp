//
// Created by rgnter on 10/11/2024.
//

#ifndef LOBBYDIRECTOR_HPP
#define LOBBYDIRECTOR_HPP

#include "../Director.hpp"

namespace alicia
{

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
