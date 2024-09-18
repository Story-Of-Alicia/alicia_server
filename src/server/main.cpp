#include <libserver/alicia.hpp>

#include <libserver/command/command_server.hpp>
#include <thread>

int main()
{
  std::jthread lobbyThread([]()
  {
    alicia::CommandServer lobbyServer;
    lobbyServer.Host("127.0.0.1", 10030);
  });

  std::jthread ranchThread([]()
  {
    alicia::CommandServer ranchServer;
    ranchServer.Host("127.0.0.1", 10031);
  });

  return 0;
}