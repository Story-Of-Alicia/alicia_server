#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

namespace alicia
{

class Settings
{
public:
  struct LobbySettings
  {
    // Bind address and port of the lobby host (default values)
    std::string address = "localhost";
    uint16_t port = 10030;
    // Advertised address and port of the ranch host (default values)
    std::string ranchAdvAddress = "localhost";
    uint16_t ranchAdvPort = 10031;
    // Advertised address and port of the messenger host (default values)
    std::string messengerAdvAddress = "localhost";
    uint16_t messengerAdvPort = 10032;
  } _lobbySettings;

  // Bind address and port of the ranch host (default values)
  struct RanchSettings
  {
    std::string address = "localhost";
    uint16_t port = 10031;
  } _ranchSettings;

  // Bind address and port of the messenger host (default values)
  struct MessengerSettings
  {
    std::string address = "localhost";
    uint16_t port = 10032;
  } _messengerSettings;

  // Updates settings from json configuration file
  void LoadFromFile(const std::filesystem::path& filePath);

  // Parse address and port from json object
  std::pair<std::string, uint16_t> ParseAddressAndPort(const nlohmann::json& jsonObject);
};

} // namespace alicia
