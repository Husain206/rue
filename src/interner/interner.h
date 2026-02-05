#pragma once
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

struct interner_id {
  std::unordered_map<std::string, uint32_t> pool;
  std::vector<std::string> from_id;
  
   uint32_t intern(const std::string& str){
    auto it = pool.find(str);
    if(it != pool.end()) return it->second;
    uint32_t id = (uint32_t)from_id.size();
    from_id.push_back(str);
    pool.emplace(from_id.back(), id);
    return id;
  }

  std::string_view view(const uint32_t id) {
    return (id < from_id.size()) ? std::string_view(from_id[id]) : std::string_view{};
  }
};

