//
// Created by rgnter on 10/11/2024.
//

#ifndef LOBBYDIRECTOR_HPP
#define LOBBYDIRECTOR_HPP

#include "../Director.hpp"
#include <server/Settings.hpp>

namespace alicia
{

class LoginDirector
{
public:
  explicit LoginDirector(CommandServer& lobbyServer, Settings settings) noexcept;

  //!
  void HandleUserLogin(ClientId clientId, const LobbyCommandLogin& login);

  //!
  void HandleHeartbeat(ClientId clientId, const LobbyCommandHeartbeat& heartbeat);

  //!
  void HandleShowInventory(ClientId clientId, const LobbyCommandShowInventory& showInventory);

  //!
  void HandleAchievementCompleteList(
    ClientId clientId,
    const LobbyCommandAchievementCompleteList& achievementCompleteList);

  //!
  void HandleRequestLeagueInfo(
    ClientId clientId,
    const LobbyCommandRequestLeagueInfo& requestLeagueInfo);

  //!
  void
  HandleRequestQuestList(ClientId clientId, const LobbyCommandRequestQuestList& requestQuestList);

  //!
  void HandleRequestSpecialEventList(
    ClientId clientId,
    const LobbyCommandRequestSpecialEventList& requestQuestList);

  //!
  void HandleEnterRanch(ClientId clientId, const LobbyCommandEnterRanch& requestEnterRanch);

  //!
  void HandleGetMessengerInfo(
    ClientId clientId,
    const LobbyCommandGetMessengerInfo& requestMessengerInfo);

private:
  CommandServer& _lobbyServer;
  Settings _settings;

  std::unordered_map<UserId, std::string> _userTokens;
  std::unordered_map<ClientId, UserId> _clients;

  std::unordered_map<UserId, User> _users;
  std::unordered_map<HorseId, HorseData> _horses;
};

} // namespace alicia

#endif // LOBBYDIRECTOR_HPP
