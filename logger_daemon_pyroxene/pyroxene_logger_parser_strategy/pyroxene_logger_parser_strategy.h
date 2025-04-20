#pragma once

#include <optional>
#include <string>
#include <chrono>
#include <vector>

namespace pyroxene_default_parser {
    std::optional<std::string> pyroxene_default_parser(const std::string& buffer);
}