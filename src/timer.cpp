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

#include <event2/event.h>

#include <chrono>

#include "timer.h"

using namespace std::chrono;

// ----------------------

static long Now(void)
{
    milliseconds ms = duration_cast< milliseconds > (steady_clock::now().time_since_epoch() );
    return ms.count();
};

// ----------------------

class Timers {
public:
    dl_list head;


    void add(Timer &t)
    {
	t.le.del();
	head.add_tail(t.le);
    };
    
    void clk(void);
};

static Timers timers;

void Timers::clk(void)
{
    long now = Now();
    list_head *pos;
    
    dl_list h;
    
    while(!head.empty()) {
	pos = head.next;
	list_del(pos);
	h.add_tail(pos);
	list_entry(pos, Timer, le)->clk(now);
    };

    head.splice(h);
};


static void timer_clk(void)
{
    timers.clk();
};

// ----------------------

void Timer::enable(long interval, bool _once)
{
    disable();
    
    once = _once;
    
    if(interval) {
	long cur = Now();
	
	if(interval > 0) {
	    period = interval;
	    dt = cur + period;
	}
	else {
	    dt = -interval;
	    if(dt <= cur)
		return;
	
	    once = true;
	    period = 0;
	};
	
	timers.add(*this);
    }
    else
	dt = period = 0;
};


void Timer::clk(long now)
{
    if(now < dt)
	return;

    if(once)
	disable();
    else {
	while(dt <= now)
	    dt += period;
    };
    
    if(on_timer)
	on_timer();
};

// ----------------------

static void timer_cb(evutil_socket_t, short, void* /*arg*/)
{
    timer_clk();
};


void timer_init(void *ev_base, int interval_us)
{
    event *ev = event_new((event_base*)ev_base, 0, EV_PERSIST, timer_cb, 0);
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = interval_us;
    evtimer_add(ev, &tv);
};

// ----------------------




void timer_once(long interval, Timer::on_timer_f f)
{
    auto t = new Timer;
    t->on_timer = [t, f]()
    {
	if(f)
	    f();
	delete t;
    };
    
    t->enable(interval, true);
};

// ----------------------

