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

#include <string>

using namespace std;

// ----------------------

extern int log_level;

// ----------------------

bool log_open(const string &filename);

void log_close(void);

void log_reopen(void);

void log_printf(int level, const char *fmt_spec, ...)  __attribute__ ((format (printf, 2, 3)));

// ----------------------

#define LOG(FMT,ARGS...) do{ log_printf(0, FMT, ## ARGS ); }while(0)

#define LOG0(FMT,ARGS...) do{ log_printf(0, FMT, ## ARGS ); }while(0)
#define LOG1(FMT,ARGS...) do{ if(log_level>=1) log_printf(1, FMT, ## ARGS ); }while(0)
#define LOG2(FMT,ARGS...) do{ if(log_level>=2) log_printf(2, FMT, ## ARGS ); }while(0)
#define LOG3(FMT,ARGS...) do{ if(log_level>=3) log_printf(3, FMT, ## ARGS ); }while(0)
#define LOG4(FMT,ARGS...) do{ if(log_level>=4) log_printf(4, FMT, ## ARGS ); }while(0)
#define LOG5(FMT,ARGS...) do{ if(log_level>=5) log_printf(5, FMT, ## ARGS ); }while(0)
#define LOG6(FMT,ARGS...) do{ if(log_level>=6) log_printf(6, FMT, ## ARGS ); }while(0)

#define XLOG1(FMT,ARGS...) do{ if(log_level>=1) log_printf(1, "%s" FMT, di(), ## ARGS ); }while(0)
#define XLOG2(FMT,ARGS...) do{ if(log_level>=2) log_printf(2, "%s" FMT, di(), ## ARGS ); }while(0)
#define XLOG3(FMT,ARGS...) do{ if(log_level>=3) log_printf(3, "%s" FMT, di(), ## ARGS ); }while(0)
#define XLOG4(FMT,ARGS...) do{ if(log_level>=4) log_printf(4, "%s" FMT, di(), ## ARGS ); }while(0)
#define XLOG5(FMT,ARGS...) do{ if(log_level>=5) log_printf(5, "%s" FMT, di(), ## ARGS ); }while(0)
#define XLOG6(FMT,ARGS...) do{ if(log_level>=6) log_printf(6, "%s" FMT, di(), ## ARGS ); }while(0)

#define BLOG(LEVEL) do { if(log_level >= LEVEL)do {
#define ELOG }while(0); }while(0);

// ----------------------


