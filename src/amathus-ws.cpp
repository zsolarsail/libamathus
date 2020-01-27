/* MIT License

Copyright (c) 2020 zsolarsail

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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include <libwebsockets.h>

#include <mutex>
#include <queue>
#include <vector>
#include <map>

#include "amathus-ws.h"

#pragma GCC diagnostic ignored "-Wunused-result"

namespace amathus {

// ----------------------

static vector<lws_protocols> protocols;

// ----------------------

class event_fd {
    int fd = -1;

    recursive_mutex mtx;
    queue<WorkFn> q;

public:
    event_fd() { fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK); };
    ~event_fd() { close(fd); };

    void notify(void);
    void reset_event(void);
    
    void push(const WorkFn& x);
    void push(WorkFn&& x);
    
    void process(void);
    
    void protocol_init(void);
    
    int get_fd(void) const { return fd; };
};

void event_fd::notify(void)
{
    uint64_t x = 1;
    write(fd, &x, sizeof(x));
};

void event_fd::reset_event(void)
{
    uint64_t x;
    read(fd, &x, sizeof(x));
};

void event_fd::push(const WorkFn& x)
{
    unique_lock<decltype(mtx)> lck(mtx);
    q.push(x);
    notify();
};

void event_fd::push(WorkFn&& x)
{
    unique_lock<decltype(mtx)> lck(mtx);
    q.push(x);
    notify();
};


void event_fd::process(void)
{
    reset_event();
    
    while(1) {
	WorkFn fn;
	{
	    unique_lock<decltype(mtx)> lck(mtx);
	    if(q.empty())
		break;
	
	    fn = std::move(q.front());
	    q.pop();
	    
	    fn();
	};
    
    };
};


static int callback_event_fd(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

void event_fd::protocol_init(void)
{
    lws_protocols p = { "event-fd", callback_event_fd, 0, 0, 0, 0 };
    protocols.push_back(p);
};


static event_fd efd;


static int callback_event_fd(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
        switch (reason) {
        case LWS_CALLBACK_PROTOCOL_INIT:
    		lws_sock_file_fd_type u;
                u.filefd = (lws_filefd_type)(long long)efd.get_fd();
                lws_adopt_descriptor_vhost(lws_get_vhost(wsi), LWS_ADOPT_RAW_FILE_DESC, u, "event-fd", NULL);
                break;

	case LWS_CALLBACK_RAW_RX_FILE:
		efd.process();
		break;
	
	default:
		break;
	};

    return 0;
};


// ----------------------


// ----------------------
// ----------------------








}; // amathus
