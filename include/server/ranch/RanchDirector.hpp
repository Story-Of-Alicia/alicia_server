//
// Created by alborrajo on 16/11/2024.
//

#ifndef RANCHDIRECTOR_HPP
#define RANCHDIRECTOR_HPP

#include "server/DataDirector.hpp"

#include "libserver/command/CommandServer.hpp"

namespace alicia
{

class RanchDirector
{
public:
  //!
  struct Settings
  {
    std::string address = "127.0.0.1";
    uint16_t port = 10031;
  };

  //!
  explicit RanchDirector(
    DataDirector& dataDirector,
    Settings settings = {});

private:
  //!
  void HandleEnterRanch(
    ClientId clientId,
    const RanchCommandEnterRanch& enterRanch);

  //!
  void HandleSnapshot(
    ClientId clientId,
    const RanchCommandRanchSnapshot& snapshot);

  //!
  void HandleCmdAction(
    ClientId clientId,
    const RanchCommandRanchCmdAction& action);

  //!
  void HandleRanchStuff(
    ClientId clientId,
    const RanchCommandRanchStuff& command);

  //!
  Settings _settings;
  //!
  DataDirector& _dataDirector;
  //!
  CommandServer _server;
  //!
  std::unordered_map<ClientId, DatumUid> _clientUsers;
};

}

#endif //RANCHDIRECTOR_HPP