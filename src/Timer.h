#pragma once


#include "Promise.h"

class Eventloop;

class Timer
{
private:
    int m_timerFd{-1};
public:
    Timer();
    ~Timer();

    Promise Wait(Eventloop& eventloop, int timeoutSeconds);
};

