#include <libserver/alicia.hpp>

int main(int argc, char** argv)
{

  if (argc > 1) {
    alicia::SetUserName(argv[1]);
  }

  int32_t id = 0;
  const auto itemParameter = [&](int idx) {
    if (argc > idx + 2) {
      alicia::AddItem(++id, std::atoi(argv[idx + 2]));
    }
  };

  if (argc > 2) {
    alicia::SetMale(std::string_view(argv[2]) == "male");
  }

  itemParameter(1);
  itemParameter(2);
  itemParameter(3);
  itemParameter(4);
  itemParameter(5);

  std::jthread loginThread([](){
    alicia::Server server;
    server.host(10030);
    server.run();
  });

  std::jthread roomThread([](){
    alicia::Server server;
    server.host(10031);
    server.run();
  });

  std::jthread ranchThread([](){
    alicia::Server server;
    server.host(10032);
    server.run();
  });

  // std::jthread messengerThread([](){
  //   alicia::Server server;
  //   server.host(10033);
  //   server.run();
  // });

  std::jthread raceThread([](){
    alicia::Server server;
    server.host(10034);
    server.run();
  });

  return 0;
}