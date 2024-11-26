// Include necessary headers
#include <string>
#include <filesystem>
#include <asio.hpp>  
#include <server/json.hpp>

class Settings
{
public:

  struct LobbySettings {
    std::string address;  // IP address or hostname
    uint16_t port = 10030;  // Default port if not specified
  } _lobbySettings;

  struct RanchSettings {
    std::string address;
    uint16_t port = 10031;
  } _ranchSettings;

  // Function to load settings from a JSON file
  void LoadFromFile(const std::filesystem::path& filePath);
  asio::ip::tcp::endpoint ResolveAddress(const std::string& host, const std::string& port);
};
