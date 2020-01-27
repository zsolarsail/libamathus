#pragma once
/*
MIT License

Copyright (c) 2016 zsolarsail

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Easy implementation of a timer for many small objects.

#include <functional>

#include "dllist.h"

using namespace std;

// ----------------------

class Timers;

class Timer {
private:
    friend class Timers;

    dl_list le;
    
    bool once = false;
    long dt = 0;
    long period = 0;

public:

    typedef std::function< void() > on_timer_f;
    
    on_timer_f on_timer;


    Timer() = default;
    Timer(const Timer &) = delete;
    void operator=(const Timer &) = delete;

    ~Timer() { disable(); };
    
    void disable(void) { le.del(); };

    void enable(long interval, bool _once=false);

private:
    void clk(long now);
};


// ----------------------

void timer_once(long interval, Timer::on_timer_f f);

// ----------------------

void timer_init(void *ev_base, int interval_us=100000);

// ----------------------

