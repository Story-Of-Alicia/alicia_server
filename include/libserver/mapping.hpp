#include <string>
#include <unordered_map>

namespace alicia
{
  const std::unordered_map<uint32_t, std::string_view>& GetMessages();
  std::string_view GetMessageName(uint32_t id);
}


