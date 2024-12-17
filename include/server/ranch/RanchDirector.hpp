//
// Created by alborrajo on 16/11/2024.
//

#ifndef RANCHDIRECTOR_HPP
#define RANCHDIRECTOR_HPP

#include "server/DataDirector.hpp"
#include "server/Settings.hpp"

#include "libserver/command/CommandServer.hpp"

#include <server/tracker/WorldTracker.hpp>

namespace alicia
{

class RanchDirector
{
public:
  //!
  explicit RanchDirector(
    DataDirector& dataDirector,
    Settings::RanchSettings settings = {});

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
  Settings::RanchSettings _settings;
  //!
  DataDirector& _dataDirector;
  //!
  CommandServer _server;

  //!
  std::unordered_map<ClientId, DatumUid> _clientCharacters;

  struct RanchInstance
  {
    WorldTracker _worldTracker;
  };
  std::unordered_map<DatumUid, RanchInstance> _ranches;
};

}

#endif //RANCHDIRECTOR_HPP
