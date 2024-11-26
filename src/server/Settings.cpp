#include <server/Settings.hpp>
#include <fstream>
#include <iostream>

void Settings::LoadFromFile(const std::filesystem::path& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open configuration file." << std::endl;
        return;
    }

    // Parse the JSON file
    nlohmann::json jsonConfig;
    file >> jsonConfig;

    // Extract lobby settings
    if (jsonConfig.contains("lobby")) {
        const auto& lobby = jsonConfig["lobby"];
        _lobbySettings.address = lobby.value("address", "127.0.0.1"); // Default to localhost
        _lobbySettings.port = lobby.value("port", 10030); // Default to 10030
    }

    if (jsonConfig.contains("ranch")) {
        const auto& lobby = jsonConfig["ranch"];
        _ranchSettings.address = lobby.value("address", "127.0.0.1"); // Default to localhost
        _ranchSettings.port = lobby.value("port", 10031); // Default to 10031
    }

}

asio::ip::tcp::endpoint Settings::ResolveAddress(const std::string& host, const std::string& port) {
    
    asio::io_context ioContext;
    asio::ip::tcp::resolver resolver(ioContext);

    // Resolve the host and port to endpoints
    auto endpoints = resolver.resolve(host, port);

    for(const auto& endpoint: endpoints)
    {
        const auto& addr = endpoint.endpoint().address();
        if (addr.is_v4())
        {
            addr.to_v4().to_uint();
        }
    }

    return *endpoints.begin();
}

