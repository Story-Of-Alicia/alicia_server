#include <libserver/alicia.hpp>
#include <libserver/proto/server.hpp>

#include <libserver/proto/lobby/messages.hpp>
#include <thread>

namespace
{

struct Player
{
  enum class Gender
  {
    Male, Female
  };

  uint32_t id = 0x00;
  std::string name{};
  Gender gender{};

  uint32_t level;
  uint32_t carrots;
  uint32_t gems;
};

class PlayerManager
{
public:
  void RegisterPlayer(Player player)
  {
    _players.emplace(player.name, std::move(player));
  }

  void HandleLoginRequest(alicia::LobbyCommandLogin& loginCommand, alicia::Client& client)
  {

  }

private:
  std::unordered_map<std::string, Player> _players;
};

struct Equipment
{
  uint32_t uniqueId;
  uint32_t modelId;
  uint32_t idk;
  uint32_t count;
};

} // namespace anon

int main()
{
  PlayerManager playerManager;
  playerManager.RegisterPlayer(Player{
    .id = 1,
    .name = "rgnt",
    .gender = Player::Gender::Male});
  playerManager.RegisterPlayer(Player{
    .id = 2,
    .name = "Laith",
    .gender = Player::Gender::Male});
  playerManager.RegisterPlayer(Player{
    .id = 3,
    .name = "Lena",
    .gender = Player::Gender::Female});

  std::jthread lobbyThread([]()
  {
    // alicia::Server lobbyServer;
    // lobbyServer.Host("127.0.0.1", 10030);
  });

  std::jthread ranchThread([]()
  {
    // alicia::Server ranchServer;
    // ranchServer.Host("127.0.0.1", 10031);
  });


  return 0;
}