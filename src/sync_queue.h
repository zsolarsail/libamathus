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

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>

using namespace std;

// ----------------------

class base_sync_queue {
protected:

    mutex mtx;

    volatile bool terminated = false;

public:
    bool pull_all = false;

    base_sync_queue() = default;
    
    base_sync_queue(const base_sync_queue&) = delete;
    void operator=(const base_sync_queue&) = delete;
    
    virtual ~base_sync_queue() = default;

    virtual bool process_queue(void) = 0;

    virtual void terminate(void);

protected:
    virtual void notify(bool all) = 0;
    
    virtual void reset_events(void) { };
};

// ----------------------

template<class T>
class tpl_sync_queue : public base_sync_queue {
protected:

    queue<T> q;

public:
    function< void(T &) > process;

    tpl_sync_queue() = default;
    tpl_sync_queue(const tpl_sync_queue&) = delete;
    void operator=(const tpl_sync_queue&) = delete;

    void push(const T& x)
    {
        unique_lock<decltype(mtx)> lck(mtx);
        q.push(x);
        notify(false);
    };

    void push(T&& x)
    {
        unique_lock<decltype(mtx)> lck(mtx);
        q.push(x);
        notify(false);
    };

    
    virtual bool get(T& obj, bool &is_term) = 0;
    
    virtual bool process_queue(void) override;
    
};

template <class T>
bool tpl_sync_queue<T>::process_queue(void)
{
    reset_events();
    bool is_term = false;

    while(!is_term) {
        T x;
        if(get(x, is_term)) {
            if(process)
                process(x);
        }
        else
            break;
    };

    return !is_term;
};


// ----------------------

class sync_queue_thread_pool {

    vector<thread*> pool;

public:
    sync_queue_thread_pool() = default;
    sync_queue_thread_pool(base_sync_queue *sq, unsigned count) { init_pool(sq, count); };
    
    ~sync_queue_thread_pool()
    {
	wait_pool();
    };

    void init_pool(base_sync_queue *sq, unsigned count);
    
    void wait_pool(void);
};


// ----------------------

template<class T>
class sync_queue : public tpl_sync_queue<T> {
private:

    condition_variable cv;

    sync_queue_thread_pool *th_pool = 0;

public:

    sync_queue() = default;
    sync_queue(const sync_queue&) = delete;
    void operator=(const sync_queue&) = delete;

    virtual ~sync_queue()
    {
	delete th_pool;
    };

    void init_thread_pool(int count=1)
    {
	th_pool = new sync_queue_thread_pool(this, count);
    };

    virtual void terminate(void) override
    {
	tpl_sync_queue<T>::terminate();
	delete th_pool;
	th_pool = 0;
    };


    virtual void notify(bool all) override
    {
	if(all)
	    cv.notify_all();
	else
	    cv.notify_one();
    };


    virtual bool get(T& obj, bool &is_term) override
    {
	unique_lock<decltype(base_sync_queue::mtx)> lck(base_sync_queue::mtx);

	while(true) {
    	    is_term = base_sync_queue::terminated;
    	    if(is_term && !base_sync_queue::pull_all)
        	break;

    	    if(!tpl_sync_queue<T>::q.empty()) {
    		if(is_term) {
    		    if(base_sync_queue::pull_all)
    			is_term = false;
    		    else
    			break;
    		};
    	    
        	obj = std::move(tpl_sync_queue<T>::q.front());
        	tpl_sync_queue<T>::q.pop();
        	return true;
    	    }
    	    else {
    		if(is_term)
    		    break;
    		else
        	    cv.wait(lck);
    	    };
	};

	return false;
    };

};



// ----------------------

