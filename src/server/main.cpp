#include <libserver/Util.hpp>
#include <libserver/command/CommandServer.hpp>
#include <libserver/base/server.hpp>

#include <memory>
#include <thread>

#include <Windows.h>

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
          .lobbyTime = alicia::UnixTimeToFileTime(
            std::chrono::system_clock::now()),

          .val0 = 0xCA794,
          .selfUid = user.id,
          .nickName = user.nickName,
          .motd = "Welcome to SoA!",
          .profileGender = user.gender,
          .status = "Mentally unstable",

          .characterEquipment = {},
          .horseEquipment = {},

          .level = user.level,
          .carrots = user.carrots,
          .val1 = 1,
          .val2 = 0,
          .val3 = 0,

          .ageGroup = alicia::AgeGroup::Adult,
          .val4 = 0,

          .val5 = {
            {0x18, 1, 2, 1},
            {0x1F, 1, 2, 1},
            {0x23, 1, 2, 1},
            {0x29, 1, 2, 1},
            {0x2A, 1, 2, 1},
            {0x2B, 1, 2, 1},
            {0x2E, 1, 2, 1}
          },

          .address = 2130706433, // 127.0.0.1
          .port = 10031, // 10031
          .scramblingConstant = 0,

          .character = {
            .parts = {
              .id = 0xA,
              .mouthSerialId =  0x01,
              .faceSerialId = 0x2,
              .val0 = 0x01},
            .appearance = {
              .val0 = 0xFFFF,
              .headSize = 0x01,
              .height = 0x01,
              .thighVolume = 0x01,
              .legVolume = 0x01,
              .val1 = 0xFF}},
          .horse = {
            .uid = 0x01,
            .tid = 0x4E21,
            .name = "idontunderstand",
            .parts = {
              .skinId = 0x1,
              .maneId = 0x4,
              .tailId = 0x4,
              .faceId = 0x5},
            .appearance = {
              .scale = 0x0,
              .legLength = 0x0,
              .legVolume = 0x0,
              .bodyLength = 0x0,
              .bodyVolume = 0x0},
            .vals0 = {
              .stamina = 0x00,
              .attractiveness = 0x00,
              .hunger = 0x00,
              .val0 = 0x00,
              .val1 = 0x03E8,
              .val2 = 0x00,
              .val3 = 0x00,
              .val4 = 0x00,
              .val5 = 0x03E8,
              .val6 = 0x1E,
              .val7 = 0x0A,
              .val8 = 0x0A,
              .val9 = 0x0A,
              .val10 = 0x00,}},
          .val7 = {
            .values = {
              {0x6, 0x0},
              {0xF, 0x4},
              {0x1B, 0x2},
              {0x1E, 0x0},
              {0x1F, 0x0},
              {0x25, 0x7530},
              {0x35, 0x4},
              {0x42, 0x2},
              {0x43, 0x4},
              {0x45, 0x0}}},
          .val8 = 0xE06,
          .val17 = {
            .val0 = 0x1,
            .val1 = 0x12}
        };

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