#include "Eventloop.h"

#include <sys/epoll.h>
#include <unistd.h>

#include <cstring>
#include <format>

#include "Log.h"

namespace
{
  Logger const LOGGER;

  struct EventData
  {
    Promise promise;
    int fd{-1};
    bool closeFDOnComplete{};
  };
}  // namespace

Eventloop::Eventloop()
{
  m_epollFd = epoll_create1(/* flags = */ 0);
  if (m_epollFd == -1)
  {
    // Log an error if we fail to create our epoll instance.
    LERROR(LOGGER, std::format("Failed to create epoll instance: {}", strerror(errno)));
    throw std::runtime_error("Failed to create epoll instance");
  }
}

Eventloop::~Eventloop()
{
  close(m_epollFd);
}

void Eventloop::AddFD(int fd, Promise promiseToSet, bool closeFDOnComplete)
{
  LDEBUG(LOGGER, std::format("Adding fd {} to eventloop", fd));
  epoll_event ev{};

  // Check for read and write events.
  ev.events = EPOLLIN | EPOLLOUT;

  // Add our data to the event so we can trigger our promise once it completes.
  ev.data.ptr = new EventData{.promise = promiseToSet, .fd = fd, .closeFDOnComplete = closeFDOnComplete};

  if (epoll_ctl(m_epollFd, /* op = */ EPOLL_CTL_ADD, /* fd = */ fd, /* event = */ &ev) == -1)
  {
    LERROR(LOGGER, std::format("Failed to add fd {} to epoll instance: {}", fd, strerror(errno)));
    throw std::runtime_error("Failed to add fd to epoll instance");
  }

  // Ensure that our eventloop is aware of outstanding work.
  m_workCount++;
}

void Eventloop::Run()
{
  // Keep waiting on events as long as we have outstanding work to do.
  while (m_workCount > 0)
  {
    // 10 is an arbitrary number of events to handle. This depends on your needs and can be tuned.
    epoll_event events[10]{};
    LTRACE(LOGGER, std::format("Work count is {}. Running epoll_wait", m_workCount));

    // Wait for events to occur. This blocks until an event has occurred.
    int ret = epoll_wait(m_epollFd, /* events = */ events, /* maxevents = */ 10, /* timeout = */ -1);
    if (ret == -1)
    {
      throw std::runtime_error("epoll_wait failed");
    }

    LDEBUG(LOGGER, std::format("Eventloop: got {} events", ret));

    for (int i = 0; i < ret; ++i)
    {
      // Get our EventData.
      auto* event = static_cast<EventData*>(events[i].data.ptr);
      LTRACE(LOGGER, std::format("Eventloop: Handling event {}", event->fd));

      // Before we trigger our promise make we sure we remove the fd from the epoll instance.
      // This is important as we'll otherwise keep getting this exact event over and over again.
      if (epoll_ctl(m_epollFd, /* op = */ EPOLL_CTL_DEL, /* fd = */ event->fd, /* event = */ nullptr) == -1)
      {
        throw std::runtime_error("Failed to remove fd from epoll instance: " + std::string(strerror(errno)));
      }

      // Trigger our promise.
      event->promise.Set();

      // Close the fd if needed.
      if (event->closeFDOnComplete)
      {
        close(event->fd);
      }

      // Delete the event-data we heap-allocated earlier in 'AddFD()'.
      delete event;

      // We've handled an event, so we can decrease our work count.
      --m_workCount;
    }
  }
}