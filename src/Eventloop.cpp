#include "Eventloop.h"

#include <sys/epoll.h>
#include <unistd.h>

#include <iostream>

struct EventData
{
  Promise promise;
  int fd{-1};
};

Eventloop::Eventloop()
{
  m_epollFd = epoll_create1(0);
  if (m_epollFd == -1)
  {
    throw std::runtime_error("Failed to create epoll instance");
  }
}

Eventloop::~Eventloop()
{
  close(m_epollFd);
}

void Eventloop::AddFD(int fd, Promise promiseToSet)
{
  std::cout << "Adding fd " << fd << " to eventloop" << std::endl;
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
    std::cout << "Work count is " << workCount << ". Running epoll_wait" << std::endl;

    int ret = epoll_wait(m_epollFd, events, 10, -1);
    if (ret == -1)
    {
      throw std::runtime_error("epoll_wait failed");
    }

    std::cout << "Eventloop: got " << ret << " events" << std::endl;

    for (int i = 0; i < ret; ++i)
    {
      auto* event = static_cast<EventData*>(events[i].data.ptr);
      std::cout << "Eventloop: Handling event " << event->fd << std::endl;
      event->promise.Set();

      if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, event->fd, nullptr) == -1)
      {
        throw std::runtime_error("Failed to remove fd from epoll instance");
      }

      delete event;

      --workCount;
    }
  }
}