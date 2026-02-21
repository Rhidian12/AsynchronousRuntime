#include "Timer.h"

#include <sys/timerfd.h>
#include <unistd.h>

#include <cstring>
#include <format>

#include "Eventloop.h"
#include "Log.h"

namespace
{
  Logger const LOGGER;
}

Promise Timer::Wait(Eventloop& eventloop, int timeoutSeconds)
{
  Promise promise;

  itimerspec timerData{};

  // Set the timer to expire after 'timeoutSeconds' seconds.
  timerData.it_value.tv_sec = timeoutSeconds;
  // Our timer should not repeat, so we set the interval to 0.
  timerData.it_value.tv_nsec = 0;

  // Our timer should not repeat, so we set the interval to 0.
  timerData.it_interval.tv_sec = 0;
  timerData.it_interval.tv_nsec = 0;

  // Create our timer file descriptor. We use CLOCK_MONOTONIC to avoid issues with system clock changes and TFD_NONBLOCK to make our timer non-blocking.
  int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
  if (timerFd == -1)
  {
    LERROR(LOGGER, std::format("Failed to create timerfd: {}", strerror(errno)));
    throw std::runtime_error("Failed to create timerfd");
  }

  // Set our timer with the earlier created 'timerData'.
  if (timerfd_settime(timerFd, 0, &timerData, nullptr) == -1)
  {
    LERROR(LOGGER, std::format("Failed to set timerfd: {}", strerror(errno)));
    throw std::runtime_error("Failed to set timerfd");
  }

  // Add our timer fd and promise to the eventloop.
  eventloop.AddFD(timerFd, promise, /* closeFDOnComplete = */ true);

  return promise;
}