//
// Created by alborrajo on 16/11/2024.
//

#ifndef RANCHDIRECTOR_HPP
#define RANCHDIRECTOR_HPP

#include "../Director.hpp"

namespace alicia
{

class RanchDirector
{
public:
  explicit RanchDirector(CommandServer& ranchServer) noexcept;

  //!
  void HandleEnterRanch(
    ClientId clientId,
    const RanchCommandEnterRanch& enterRanch);

  void HandleSnapshot(
    ClientId clientId,
    const RanchCommandRanchSnapshot& snapshot);

  void HandleCmdAction(ClientId clientId, const RanchCommandRanchCmdAction& action);

  void HandleRanchStuff(ClientId clientId, const RanchCommandRanchStuff& command);

private:
  CommandServer& _ranchServer;

  std::unordered_map<ClientId, UserId> _clients{};
  std::unordered_map<UserId, User> _users{};
  std::unordered_map<RanchId, Ranch> _ranches{};
};

}

#endif //RANCHDIRECTOR_HPP