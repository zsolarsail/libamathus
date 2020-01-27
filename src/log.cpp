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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>


#include "sync_queue.h"

#include "log.h"

// ----------------------

int log_level = 3;

static string log_filename;
static FILE *log_file = 0;


// ----------------------

struct log_message {
    enum MsgType { Message, Reopen };
    
    string message;
    MsgType type = Message;
    
    log_message() = default;
    
    log_message(const string &msg) : message(msg) { };
    
};

static sync_queue<log_message> log_queue;

// ----------------------

static void int_log_reopen(void);

// ----------------------

static void on_log_message(log_message &msg)
{
    if(!log_file)
	return;

    switch(msg.type) {
	case log_message::Message:
	    fwrite(msg.message.c_str(), msg.message.length(), 1, log_file);
	    fflush(log_file);
	    break;

	case log_message::Reopen:
	    int_log_reopen();
	    break;
	
	default:
	    break;
    };
};

// ----------------------

bool log_open(const string &filename)
{
    log_close();
    log_file = fopen(filename.c_str(), "a");
    
    if(log_file) {
	log_filename = filename;
	log_queue.pull_all = true;
	log_queue.process = on_log_message;
	log_queue.init_thread_pool();
	
	return true;
    };
    
    return false;
};

// ----------------------

void log_close(void)
{
    if(log_file) {
	log_queue.terminate();
	fclose(log_file);
	log_file = 0;
	log_filename.erase();
    };
};

// ----------------------

static void int_log_reopen(void)
{
    if(log_file) {
	fclose(log_file);
	log_file = fopen(log_filename.c_str(), "a");
    };
};


// ----------------------

void log_printf(int level, const char *fmt_spec, ...)
{
    timeval st;
    struct timezone tz;

    if(level > log_level)
        return;

    if(!log_file)
	return;

    log_message m;
    
     if(!fmt_spec || !fmt_spec[0]) {
        m.message = '\n';
     } else {
	gettimeofday(&st, &tz);

	time_t tu = st.tv_sec;
	tm t;
	localtime_r(&tu, &t);
	char buf[64*1024];

	auto len = sprintf(buf, "%02d.%02d.%04d-%02d:%02d:%02d.%06d: ", t.tm_mday,
    	    t.tm_mon+1, t.tm_year+1900, t.tm_hour, t.tm_min, t.tm_sec, (int)st.tv_usec);

	m.message.append(buf, len);

	va_list arglist;
	va_start (arglist, fmt_spec);
	len = vsnprintf(buf, sizeof(buf)-1, fmt_spec, arglist);
	va_end(arglist);
	
	m.message.append(buf, len);
	m.message += '\n';
     };
     
     log_queue.push(std::move(m));
};

// ----------------------

void log_reopen(void)
{
    log_message m;
    m.type = log_message::Reopen;
    log_queue.push(std::move(m));
};

// ----------------------



