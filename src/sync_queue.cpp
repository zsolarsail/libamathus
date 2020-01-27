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

void base_sync_queue::terminate(void)
{
    unique_lock<decltype(mtx)> lck(mtx);
    terminated = true;
    notify(true);
};

// ----------------------

void sync_queue_thread_pool::init_pool(base_sync_queue *sq, unsigned count)
{
    if(count == 0)
	count = 1;

    while(count--)
	pool.push_back( new thread(
	    [sq](void) { 
		while(sq->process_queue());
	    }) 
	);
};

void sync_queue_thread_pool::wait_pool(void)
{
    for(auto &t: pool)
	t->join();

    pool.clear();
};


// ----------------------
