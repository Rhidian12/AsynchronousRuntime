#include "Log.h"

#include <chrono>
#include <format>
#include <iostream>

constexpr std::string_view LOG_STRINGS[] = {"TRACE", "DEBUG", "INFO", "WARNING", "ERROR"};

void Logger::Log(LogLevel level, std::string_view message) const
{
  if (level < m_level)
  {
    return;
  }

  std::cout << std::format("[{}] - [{}]: {}", std::chrono::system_clock::now(), LOG_STRINGS[static_cast<int>(level)], message) << std::endl;
}