#include "timer.h"

unsigned int timer_start()
{
    return _CP0_GET_COUNT();
}

unsigned char timer_timeout(unsigned int ts, unsigned int to)
{
    unsigned int tout = ts + to;
    unsigned int t = _CP0_GET_COUNT();
    if (ts<tout) //no roll over for the timer 
    {
        if (tout < t)
        {
            return TIMER_DONE;
        }
        if (t < ts) // rollover for the counter
        {
            return TIMER_DONE;
        }
        return TIMER_PENDING;
    }
    // roll over for the timer 
    if (t > ts)
    {
        return TIMER_PENDING;
    }
    if (t < tout)
    {
        return TIMER_PENDING;
    }
    return TIMER_DONE;
}