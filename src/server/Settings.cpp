#include <fstream>
#include <iostream>
#include <server/Settings.hpp>

namespace alicia
{

void Settings::LoadFromFile(const std::filesystem::path& filePath)
{
  std::ifstream file(filePath);

  if (!file.is_open())
  {
    std::cerr << "Error: Could not open configuration file." << std::endl;
    return;
  }

  // Parse the JSON file
  nlohmann::json jsonConfig;
  file >> jsonConfig;

  // Extract lobby settings
  if (jsonConfig.contains("lobby"))
  {
    const auto& lobby = jsonConfig["lobby"];
    _lobbySettings.address = lobby.value("address", "localhost"); // Default to localhost
    _lobbySettings.port = lobby.value("port", 10030);             // Default to 10030
  }

  if (jsonConfig.contains("ranch"))
  {
    const auto& lobby = jsonConfig["ranch"];
    _ranchSettings.address = lobby.value("address", "localhost"); // Default to localhost
    _ranchSettings.port = lobby.value("port", 10031);             // Default to 10031
  }
}

} // namespace alicia
