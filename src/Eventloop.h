#pragma once

#include "Promise.h"

class Eventloop
{
 private:
  // How much oustanding work do we have?
  int m_workCount{0};

  // The file descriptor for our epoll instance.
  int m_epollFd{-1};

 public:
  Eventloop();
  ~Eventloop();

  // Our eventloop is not copyable or moveable. Our epoll instance is unique to this eventloop.
  Eventloop(Eventloop const&) = delete;
  Eventloop& operator=(Eventloop const&) = delete;
  Eventloop(Eventloop&&) = delete;
  Eventloop& operator=(Eventloop&&) = delete;

  // Add an event to the eventloop. The promise will be set once the event completes.
  void AddFD(int fd, Promise promiseToSet, bool closeFDOnComplete);

  // Run the event loop until no work remains on it
  void Run();
};