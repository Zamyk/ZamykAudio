#pragma once

#include <string>
#include <optional>
#include <algorithm>

namespace ZAudio::StringTools {


std::optional<double> stringToDouble(const std::string& str);
std::optional<int64_t> stringToInt(const std::string& str);
bool caseInsensitiveEqual(const std::string& str1, const std::string& str2);


} // namespace ZAudio::StringTools