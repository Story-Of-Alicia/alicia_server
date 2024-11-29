//
// Created by rgnter on 25/11/2024.
//

#ifndef LOBBYDIRECTOR_HPP
#define LOBBYDIRECTOR_HPP

#include "LoginHandler.hpp"
#include "server/DataDirector.hpp"

#include "libserver/command/CommandServer.hpp"

namespace alicia
{

class LobbyDirector
{
public:
  //!
  struct Settings
  {
    std::string address = "127.0.0.1";
    uint16_t port = 10030;

    std::string motd = "Welcome to Story of Alicia!";
  };

  //!
  explicit LobbyDirector(
    DataDirector& dataDirector,
    Settings settings = {});

private:
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

  //!
  void HandleRequestSpecialEventList(
    ClientId clientId, const
    LobbyCommandRequestSpecialEventList& requestQuestList);

  //!
  void HandleEnterRanch(
    ClientId clientId, const
    LobbyCommandEnterRanch& requestEnterRanch);

  //!
  void HandleGetMessengerInfo(
    ClientId clientId, const
    LobbyCommandGetMessengerInfo& requestMessengerInfo);

  //!
  Settings _settings;

  //!
  DataDirector& _dataDirector;
  //!
  LoginHandler _loginHandler;

  //!
  CommandServer _server;
  //!
  std::unordered_map<ClientId, DatumUid> _clientUsers;

  //!
  struct ClientLoginContext
  {
    std::string userName;

    DatumUid characterUid{InvalidDatumUid};
    LobbyCommandLoginOK response{};
  };

  //!
  std::unordered_map<ClientId, ClientLoginContext> _queuedClientLogins;
};

}

#endif //LOBBYDIRECTOR_HPP
