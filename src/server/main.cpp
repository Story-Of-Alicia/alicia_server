#include <libserver/alicia.hpp>

int main()
{
  std::thread loginThread([](){
    alicia::Server server;
    server.host(10030);
    server.run();
  });

  std::thread roomThread([](){
    alicia::Server server;
    server.host(10031);
    server.run();
  });

  std::thread ranchThread([](){
    alicia::Server server;
    server.host(10032);
    server.run();
  });

  std::thread messengerThread([](){
    alicia::Server server;
    server.host(10033);
    server.run();
  });

  std::thread raceThread([](){
    alicia::Server server;
    server.host(10034);
    server.run();
  });

  loginThread.join();
  roomThread.join();
  ranchThread.join();
  messengerThread.join();
  raceThread.join();

  return 0;
}