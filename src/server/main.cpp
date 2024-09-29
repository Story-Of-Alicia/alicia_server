#include <libserver/alicia.hpp>

#include <libserver/command/command_server.hpp>
#include <thread>

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
    const auto& userTokenItr = _userTokens.find(login.memberNo);

    // If the user does not have an active token,
    // or the token is invalid - cancel the login.
    if (userTokenItr == _userTokens.cend()
      || login.authKey != userTokenItr->second)
    {
      _lobbyServer.QueueCommand(
        clientId,
        alicia::CommandId::LobbyCommandLoginOK,
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
      [&user]()
    {
      return alicia::LobbyCommandLoginOK {
        .lobbyTime = 0,
        .selfUid = user.id,
        .nickName = user.nickName,
        .profileGender = user.gender,
        .level = user.level,
        .carrots = user.carrots};
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
  std::jthread lobbyThread([]()
  {
    alicia::CommandServer lobbyServer;
    lobbyServer.Host("127.0.0.1", 10030);

    g_loginDirector = std::make_unique<>(lobbyServer);

    lobbyServer.RegisterCommandHandler(
      alicia::CommandId::LobbyCommandLogin,
      [&](auto& buffer)
      {
        alicia::LobbyCommandLogin loginCommand;
        alicia::LobbyCommandLogin::Read(
          loginCommand, buffer);

        g_loginDirector.HandleUserLogin(loginCommand);
      });
  });

  std::jthread ranchThread([]()
  {
    alicia::CommandServer ranchServer;
    ranchServer.Host("127.0.0.1", 10031);
  });

  return 0;
}