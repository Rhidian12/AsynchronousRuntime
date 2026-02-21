
#include "Eventloop.h"
#include "Promise.h"
#include "Timer.h"
#include "Log.h"

Logger const LOGGER;

Promise Work(Eventloop& eventloop)
{
  Timer timer;
  LINFO(LOGGER, "Work: First timer");
  co_await timer.Wait(eventloop, 1);

  LINFO(LOGGER, "Work: Second timer");
  co_await timer.Wait(eventloop, 2);
  LINFO(LOGGER, "Work: Done waiting 2 seconds");
  co_return;
}

Promise Work2(Eventloop& eventloop)
{
  Timer timer;

  LINFO(LOGGER, "Work2: First timer");
  co_await timer.Wait(eventloop, 1);

  LINFO(LOGGER, "Work2: Second timer");
  co_await timer.Wait(eventloop, 2);
  LINFO(LOGGER, "Work2: Done waiting 2 seconds");
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
  LINFO(LOGGER, "Creating eventloop");
  Eventloop eventloop;

  LINFO(LOGGER, "Starting AsyncMain");
  auto promise = AsyncMain(eventloop);

  LINFO(LOGGER, "Running eventloop");
  eventloop.Run();

  return 0;
}