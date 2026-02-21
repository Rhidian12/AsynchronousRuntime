#include "Timer.h"

#include <sys/timerfd.h>
#include <unistd.h>

#include <format>

#include "Eventloop.h"
#include "Log.h"

namespace
{
  Logger const LOGGER;
}

Timer::Timer() {}

Timer::~Timer()
{
  LDEBUG(LOGGER, std::format("Closing timer fd {}", m_timerFd));
  close(m_timerFd);
}

Promise Timer::Wait(Eventloop& eventloop, int timeoutSeconds)
{
  Promise promise;

  itimerspec val{};

  val.it_value.tv_sec = timeoutSeconds;
  val.it_value.tv_nsec = 0;

  val.it_interval.tv_sec = 0;
  val.it_interval.tv_nsec = 0;

  m_timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
  if (m_timerFd == -1)
  {
    LERROR(LOGGER, "Failed to create timerfd");
    throw std::runtime_error("Failed to create timerfd");
  }
  if (timerfd_settime(m_timerFd, 0, &val, nullptr) == -1)
  {
    LERROR(LOGGER, "Failed to set timerfd");
    throw std::runtime_error("Failed to set timerfd");
  }

  eventloop.AddFD(m_timerFd, promise);

  return promise;
}