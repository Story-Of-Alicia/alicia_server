//
// Created by alborrajo on 16/11/2024.
//

#ifndef RANCHDIRECTOR_HPP
#define RANCHDIRECTOR_HPP

#include "../Director.hpp"
#include <server/Settings.hpp>

namespace alicia
{

class RanchDirector
{
public:
  explicit RanchDirector(CommandServer& ranchServer, Settings settings) noexcept;

  //!
  void HandleEnterRanch(ClientId clientId, const RanchCommandEnterRanch& enterRanch);

  void HandleSnapshot(ClientId clientId, const RanchCommandRanchSnapshot& snapshot);

  void HandleCmdAction(ClientId clientId, const RanchCommandRanchCmdAction& action);

  void HandleRanchStuff(ClientId clientId, const RanchCommandRanchStuff& command);

  void HandleUpdateBusyState(ClientId clientId, const RanchCommandUpdateBusyState& command);

  void HandleSearchStallion(ClientId clientId, const RanchCommandSearchStallion& command);
  void HandleEnterBreedingMarket(ClientId clientId, const RanchCommandEnterBreedingMarket& command);
  void HandleTryBreeding(ClientId clientId, const RanchCommandTryBreeding& command);
  void HandleBreedingWishlist(ClientId clientId, const RanchCommandBreedingWishlist& command);
  void HandleUpdateMountNickname(ClientId clientId, const RanchCommandUpdateMountNickname& command);

private:
  CommandServer& _ranchServer;
  Settings _settings;
  std::unordered_map<ClientId, UserId> _clients{};

  std::unordered_map<UserId, User> _users{};
  std::unordered_map<HorseId, HorseData> _horses{};
  std::unordered_map<RanchId, Ranch> _ranches{};
};

} // namespace alicia

#endif // RANCHDIRECTOR_HPP
