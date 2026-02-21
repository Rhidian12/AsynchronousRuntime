#include <iostream>

#include "Eventloop.h"
#include "Promise.h"
#include "Timer.h"

Promise Work(Eventloop& eventloop)
{
  Timer timer;
  std::cout << "Work: First timer" << std::endl;
  co_await timer.Wait(eventloop, 1);

  std::cout << "Work: Second timer" << std::endl;
  co_await timer.Wait(eventloop, 2);
  std::cout << "Work: Done waiting 2 seconds" << std::endl;
  co_return;
}

Promise Work2(Eventloop& eventloop)
{
  Timer timer;

  std::cout << "Work2: First timer" << std::endl;
  co_await timer.Wait(eventloop, 1);

  std::cout << "Work2: Second timer" << std::endl;
  co_await timer.Wait(eventloop, 2);
  std::cout << "Work2: Done waiting 2 seconds" << std::endl;
  co_return;
}

Promise AsyncMain(Eventloop& eventloop)
{
  // Start some detached coroutines
  std::ignore = Work(eventloop);
  std::ignore = Work2(eventloop);

  co_return;
}

int main()
{
  std::cout << "Creating eventloop" << std::endl;
  Eventloop eventloop;

  std::cout << "Starting AsyncMain" << std::endl;
  auto promise = AsyncMain(eventloop);

  std::cout << "Running eventloop" << std::endl;
  eventloop.Run();

  return 0;
}