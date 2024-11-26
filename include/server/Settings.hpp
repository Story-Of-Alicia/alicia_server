#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

namespace alicia
{

class Settings
{
public:
  struct LobbySettings
  {
    std::string address;   // IP address or hostname
    uint16_t port = 10030; // Default port if not specified
  } _lobbySettings;

  struct RanchSettings
  {
    std::string address;
    uint16_t port = 10031;
  } _ranchSettings;

  void LoadFromFile(const std::filesystem::path& filePath);
};

} // namespace alicia
