#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <filesystem>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

namespace alicia
{

class Settings
{
public:
  struct LobbySettings
  {
    // Bind address and port of the lobby host.
    std::string address = "127.0.0.1";
    uint16_t port = 10030;

    // Advertised address and port of the ranch host.
    std::string ranchAdvAddress = "127.0.0.1";
    uint16_t ranchAdvPort = 10031;

    // Advertised address and port of the messenger host.
    std::string messengerAdvAddress = "127.0.0.1";
    uint16_t messengerAdvPort = 10032;
  } _lobbySettings;

  // Bind address and port of the ranch host.
  struct RanchSettings
  {
    std::string address = "127.0.0.1";
    uint16_t port = 10031;
  } _ranchSettings;

  // Bind address and port of the messenger host.
  struct MessengerSettings
  {
    std::string address = "127.0.0.1";
    uint16_t port = 10032;
  } _messengerSettings;

  // Updates settings from json configuration file
  void LoadFromFile(const std::filesystem::path& filePath);

  // Parse address and port from json object
  std::pair<std::string, uint16_t> ParseAddressAndPort(const nlohmann::json& jsonObject);
};

} // namespace alicia

#endif // SETTINGS_HPP