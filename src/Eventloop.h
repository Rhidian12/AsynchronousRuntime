#pragma once

#include "Promise.h"

class Eventloop
{
 private:
 int workCount{0};
  int m_epollFd{-1};

 public:
  Eventloop();
  ~Eventloop();

  void AddFD(int fd, Promise promiseToSet);

  // Run the event loop until no work remains on it
  void Run();
};