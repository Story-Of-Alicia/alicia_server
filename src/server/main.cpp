#include <libserver/alicia.hpp>

#include <libserver/command/command_server.hpp>
#include <thread>

namespace
{

using UserId = std::uint32_t;

struct User
{
  std::string nickName;
  uint16_t level{};
  int32_t carrots{};
};

class LoginDirector
{
public:
  LoginDirector()
  {
    _userTokens[1] = "test";
  }

  bool AuthorizeUser(UserId user, const std::string& providedToken)
  {
    const auto tokenItr = _userTokens.find(user);
    if (tokenItr == _userTokens.cend())
    {
      // User doesn't have any tokens.
      return false;
    }

    const auto& token = tokenItr->second;
    if (token == providedToken)
    {
      // The tokens match.
      return true;
    }

    return false;
  }

  User& GetUser(UserId user)
  {
    return _users[user];
  }

private:
  std::unordered_map<UserId, std::string> _userTokens;
  std::unordered_map<UserId, User> _users;
} g_loginDirector;

}

int main()
{
  std::jthread lobbyThread([]()
  {
    alicia::CommandServer lobbyServer;
    lobbyServer.Host("127.0.0.1", 10030);
    lobbyServer.RegisterCommandHandler(
      alicia::CommandId::LobbyCommandLogin,
      [&](auto& buffer)
      {
        alicia::LobbyCommandLogin loginCommand;
        alicia::LobbyCommandLogin::Read(
          loginCommand, buffer);

        const bool result = g_loginDirector.AuthorizeUser(
          loginCommand.memberNo, loginCommand.authKey);
        if (result)
        {
          const auto& user = g_loginDirector.GetUser(
            loginCommand.memberNo);

          lobbyServer.SendCommand([&](auto& buffer)
          {
            const alicia::LobbyCommandLoginOK loginOkCommand {
              .lobbyTime = 0 /* todo: win filetime*/,
              .selfUid = loginCommand.memberNo,
              .nickName = user.nickName,
              .level = user.level,
              .carrots = user.carrots};

            alicia::LobbyCommandLoginOK::Write(loginOkCommand, buffer);
          });
        }
      });
  });

  std::jthread ranchThread([]()
  {
    alicia::CommandServer ranchServer;
    ranchServer.Host("127.0.0.1", 10031);
  });

  return 0;
}