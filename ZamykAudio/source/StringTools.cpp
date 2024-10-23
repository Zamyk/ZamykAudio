#include <ZAudio/StringTools.h>

namespace ZAudio::StringTools {
  

std::optional<double> stringToDouble(const std::string& str) {
  char* end = nullptr;
  double result = strtod(str.c_str(), &end);

  if (end == str.c_str() || *end != '\0') {
    return std::nullopt;
  }

  return result;
}

std::optional<int64_t> stringToInt(const std::string& str) {
  char* end = nullptr;
  int64_t result = strtoll(str.c_str(), &end, 10);

  if (end == str.c_str() || *end != '\0') {
    return std::nullopt;
  }

  return result;
}

bool caseInsensitiveEqual(const std::string& str1, const std::string& str2) {
  return std::equal(str1.begin(), str1.end(), str2.begin(), str2.end(), 
    [] (char a, char b) {
      return std::tolower(a) == std::tolower(b);
    }
  );
}



} // namespace ZAudio::StringTools