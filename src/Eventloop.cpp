#include "Eventloop.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <format>

#include <cstring>

#include "Log.h"

namespace
{
  Logger const LOGGER;

  struct EventData
  {
    Promise promise;
    int fd{-1};
  };
}  // namespace

Eventloop::Eventloop()
{
  m_epollFd = epoll_create1(0);
  if (m_epollFd == -1)
  {
    LERROR(LOGGER, "Failed to create epoll instance");
    throw std::runtime_error("Failed to create epoll instance");
  }
}

Eventloop::~Eventloop()
{
  close(m_epollFd);
}

void Eventloop::AddFD(int fd, Promise promiseToSet)
{
  LDEBUG(LOGGER, std::format("Adding fd {} to eventloop", fd));
  epoll_event ev{};
  ev.events = EPOLLIN;
  ev.data.ptr = new EventData{promiseToSet, fd};
  workCount++;

  if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &ev) == -1)
  {
    throw std::runtime_error("Failed to add fd to epoll instance");
  }
}

void Eventloop::Run()
{
  while (workCount > 0)
  {
    epoll_event events[10]{};
    LDEBUG(LOGGER, std::format("Work count is {}. Running epoll_wait", workCount));

    int ret = epoll_wait(m_epollFd, events, 10, -1);
    if (ret == -1)
    {
      throw std::runtime_error("epoll_wait failed");
    }

    LDEBUG(LOGGER, std::format("Eventloop: got {} events", ret));

    for (int i = 0; i < ret; ++i)
    {
      auto* event = static_cast<EventData*>(events[i].data.ptr);
      LTRACE(LOGGER, std::format("Eventloop: Handling event {}", event->fd));

      if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, event->fd, nullptr) == -1)
      {
        throw std::runtime_error("Failed to remove fd from epoll instance: " + std::string(strerror(errno)));
      }

      event->promise.Set();
      delete event;

      --workCount;
    }
  }
}