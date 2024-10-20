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
    assert(login.constant0 == 50);
    assert(login.constant1 == 281);

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
        [](alicia::SinkStream& buffer)
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
      [&user](alicia::SinkStream& sink)
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
  boost::asio::streambuf buf;
  auto mutableBuffer = buf.prepare(4092);
  alicia::SinkStream sink(std::span(
    static_cast<std::byte*>(mutableBuffer.data()),
    mutableBuffer.size()));

  sink.Write(0xCAFE);
  sink.Write(0xBABE);

  assert(sink.GetCursor() == 8);
  buf.commit(sink.GetCursor());

  auto constBuffer = buf.data();
  alicia::SourceStream source(std::span(
    static_cast<const std::byte*>(constBuffer.data()),
    constBuffer.size()));

  uint32_t cafe{};
  uint32_t babe{};
  source.Read(cafe)
    .Read(babe);
  assert(cafe == 0xCAFE && babe == 0xBABE);
  assert(source.GetCursor() == 8);

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