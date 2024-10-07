#include <libserver/alicia.hpp>
#include <libserver/command/CommandServer.hpp>

#include <memory>
#include <thread>

#include <libserver/base/server.hpp>

namespace
{

//! Unique user identifier.
using UserId = std::uint32_t;
//! Invalid user identifier.
constexpr UserId InvalidUserId = std::numeric_limits<UserId>::max();

struct User
{
  UserId id{};
  std::string nickName;
  alicia::Gender gender;

  uint16_t level{};
  int32_t carrots{};
};

class LoginDirector
{
public:
  LoginDirector(alicia::CommandServer& lobbyServer)
    : _lobbyServer(lobbyServer)
  {
    // TODO: Dummy data
    _userTokens[1] = "test";
    _users[1] = {
      .id = 1,
      .nickName = "rgnt",
      .gender = alicia::Gender::Boy,
      .level = 60,
      .carrots = 5000,};
  }

  void HandleUserLogin(
    alicia::ClientId clientId,
    const alicia::LobbyCommandLogin& login)
  {
    assert(login.constant0 == 26482);
    assert(login.constant1 == 29806);

    // ToDo: Treat input.
    const auto& userTokenItr = _userTokens.find(login.memberNo);

    // If the user does not have an active token,
    // or the token is invalid - cancel the login.
    if (userTokenItr == _userTokens.cend()
      || login.authKey != userTokenItr->second)
    {
      _lobbyServer.QueueCommand(
        clientId,
        alicia::CommandId::LobbyCommandLoginCancel,
        [](alicia::SinkBuffer& buffer)
        {
          const alicia::LobbyCommandLoginCancel command{
            .reason = alicia::LoginCancelReason::InvalidUser};

          alicia::LobbyCommandLoginCancel::Write(
            command,
            buffer);
        });

      return;
    }

    // TODO: Get the user from data source if needed.
    const auto& user = _users[login.memberNo];

    // The token is valid, accept the login.
    _lobbyServer.QueueCommand(
      clientId,
      alicia::CommandId::LobbyCommandLoginOK,
      [&user](alicia::SinkBuffer& sink)
      {
        const alicia::LobbyCommandLoginOK command{
           .lobbyTime = 0,
           .selfUid = user.id,
           .nickName = user.nickName,
           .profileGender = user.gender,
           .level = user.level,
           .carrots = user.carrots};
        alicia::LobbyCommandLoginOK::Write(command, sink);
      });
  }

  User& GetUser(UserId user)
  {
    return _users[user];
  }

private:
  alicia::CommandServer& _lobbyServer;

  std::unordered_map<UserId, std::string> _userTokens;
  std::unordered_map<UserId, User> _users;
};

std::unique_ptr<LoginDirector> g_loginDirector;

}

int main()
{
  alicia::Buffer<4096> buffer;

  uint64_t val { 0xDEAD'BEEF'CAFE'BABE };

  buffer.Write(&val, sizeof val);
  buffer.Seek(0);

  uint32_t val0 {};
  uint32_t val1 {};

  buffer.Read(&val0, sizeof val0);
  assert(buffer.GetCursor() == sizeof val0);
  buffer.Read(&val1, sizeof val1);
  assert(buffer.GetCursor() == sizeof val0 + sizeof val1);

  assert(buffer.GetCursor() == sizeof val);
  // Little endian!
  assert(val1 == 0xDEAD'BEEF);
  assert(val0 == 0xCAFE'BABE);

  boost::asio::streambuf buf;
  std::iostream output(&buf);

  uint32_t t = 1;
  output.write(reinterpret_cast<const char*>(&t), 4);
  assert(output.good());

  std::jthread lobbyThread([]()
  {
    alicia::CommandServer lobbyServer;
    g_loginDirector = std::make_unique<LoginDirector>(lobbyServer);

    // Handlers
    lobbyServer.RegisterCommandHandler(
      alicia::CommandId::LobbyCommandLogin,
      [](alicia::ClientId clientId, auto& buffer)
      {
        alicia::LobbyCommandLogin loginCommand;
        alicia::LobbyCommandLogin::Read(
          loginCommand, buffer);

        g_loginDirector->HandleUserLogin(clientId, loginCommand);
      });

    // Host
    lobbyServer.Host("127.0.0.1", 10030);
    printf("Lobby server running on loopback port %d\n", 10030);
  });

  std::jthread ranchThread([]()
  {
    alicia::CommandServer ranchServer;
    ranchServer.Host("127.0.0.1", 10031);
    printf("Ranch server running on loopback port %d\n", 10031);
  });

  return 0;
}