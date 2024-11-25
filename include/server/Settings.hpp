// Include necessary headers
#include <string>
#include <filesystem>
#include <asio.hpp>  // For networking and address resolution

// Include JSON parsing library (you might use a library like nlohmann/json)
#include <server/json.hpp>

class Settings
{
public:
  // Struct to hold lobby settings
  struct LobbySettings {
    std::string address;  // IP address or hostname
    uint16_t port = 10030;  // Default port if not specified
  } _lobbySettings;

  // Other settings can go here (like RanchSettings if needed)
  struct RanchSettings {
    // Add parameters here if needed
  } _ranchSettings;

  // Function to load settings from a JSON file
  void LoadFromFile(const std::filesystem::path& filePath);
  asio::ip::tcp::endpoint ResolveAddress(const std::string& host, const std::string& port);
private:
  // Add any additional helper functions or member variables if needed
};
