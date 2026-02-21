#pragma once

#include "Promise.h"

class Eventloop;

class Timer
{
 public:
  Timer() = default;

  // Promises should not be discarded.
  [[nodiscard]] Promise Wait(Eventloop& eventloop, int timeoutSeconds);
};
