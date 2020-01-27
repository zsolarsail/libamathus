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

#include "sync_queue.h"

// ----------------------

class event_fd {

    int fd = -1;

public:

    event_fd();
    event_fd(const event_fd&) = delete;
    void operator=(const event_fd&) = delete;

    ~event_fd();

    int get_fd(void) const { return fd; };

    void notify(void);
    void reset_events(void);
};

// ----------------------

void eventfd_queue_init(void *_eb, int fd, base_sync_queue *sq);

// ----------------------

template<class T>
class eventfd_queue : public tpl_sync_queue<T> {
private:

    event_fd efd;

public:

    eventfd_queue() = default;
    eventfd_queue(const eventfd_queue&) = delete;
    void operator=(const eventfd_queue&) = delete;

    virtual bool get(T& obj, bool &is_term) override
    {
	unique_lock<decltype(base_sync_queue::mtx)> lck(base_sync_queue::mtx);

	is_term = base_sync_queue::terminated;
	if(is_term)
    	    return false;

	if(!tpl_sync_queue<T>::q.empty()) {
    	    obj = std::move(tpl_sync_queue<T>::q.front());
    	    tpl_sync_queue<T>::q.pop();
    	    return true;
	};

	return false;    
    };


    void set_event_base(void *eb)
    {
	eventfd_queue_init(eb, efd.get_fd(), this);
    };

protected:

     virtual void notify(bool) override
     {
        efd.notify();
     };

    virtual void reset_events(void) override
    {
	efd.reset_events();
    };
};

// ----------------------

