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

#include <unistd.h>
#include <sys/eventfd.h>
#include <event2/event.h>

#include "eventfd_queue.h"

#pragma GCC diagnostic ignored "-Wunused-result"

// ----------------------

event_fd::event_fd()
{
    fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
};


event_fd::~event_fd()
{
    close(fd);
};

void event_fd::notify(void)
{
    uint64_t x = 1;
    write(fd, &x, sizeof(x));
};

void event_fd::reset_events(void)
{
    uint64_t x;
    read(fd, &x, sizeof(x));
};

// ----------------------

static void efd_queue_cb(evutil_socket_t , short, void *arg)
{
    ((base_sync_queue*)arg)->process_queue();
};


void eventfd_queue_init(void *_eb, int fd, base_sync_queue *sq)
{
    auto eb = (event_base*)_eb;
    
    event *ev = event_new(eb, fd, EV_READ | EV_PERSIST, efd_queue_cb, sq);
    event_add(ev, 0);
};


// ----------------------


