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

#pragma once

#include "dllisth.h"



/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#if 0
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
#else
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)+0x1000-(unsigned long)(&((type *)0x1000)->member)))
#endif

#define OFFSETOF(type, member) ((unsigned long)(&((type *)0)->member))


/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define list_for_each_no_init(pos, head) \
	for (; pos != (head); pos = pos->next)

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)


#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
	    pos = n, n = pos->next)



// ----------------------
// A pool of objects in which there must be a field le (dl_list).
template <class T, unsigned int Delta=1024>
class dllist_pool {
private:
    
    struct block
    {
        block *next;
        T data[Delta];
      
        block(dl_list &head, block *_next) : next(_next)
        {
            for(auto &x: data)
                head.add_tail(x.le);
        };
    };
    

    block *blocks = 0;

    dl_list head;

public:
    dllist_pool() = default;
    dllist_pool(const dllist_pool &) = delete;
    void operator = (const dllist_pool &) = delete;

    ~dllist_pool()
    {
        while(blocks)
        {
            block *tmp = blocks;
            blocks = blocks->next;
            delete tmp;
        };
    };
        
    T* alloc(void);

    void free(T *node)
    {
	node->le.del();
	head.add(node->le);
    };
};

template <class T, unsigned int Delta>
T* dllist_pool<T,Delta>::alloc(void)
{   
    if(head.empty())
        blocks = new block(head, blocks);
        
    T* node = list_entry(head.next, T, le);
    node->le.del();
    return node;
};  


// ----------------------

template <class T>
class dl_list_iterator {
public:
    list_head *h = 0;

    dl_list_iterator() = delete;
    dl_list_iterator(list_head *_h) : h(_h) { };
    dl_list_iterator(list_head &_h) : h(&_h) { };
    
    dl_list_iterator(const dl_list_iterator &x) : h(x.h) { };

    dl_list_iterator& operator++()
    {
	h = h->next;
	return *this;
    };

    dl_list_iterator operator++(int)
    {
	dl_list_iterator tmp(this);
	h = h->next;
	return tmp;
    };
    
    bool operator==(const dl_list_iterator &x) { return h == x.h; };
    bool operator!=(const dl_list_iterator &x) { return h != x.h; };
    
    T& operator*() { return *list_entry(h, T, le); };
};

template <class T>
class dl_list_const_iterator {
public:
    const list_head *h = 0;

    dl_list_const_iterator() = delete;
    dl_list_const_iterator(const list_head *_h) : h(_h) { };
    dl_list_const_iterator(const list_head &_h) : h(&_h) { };
    
    dl_list_const_iterator(const dl_list_const_iterator &x) : h(x.h) { };

    dl_list_const_iterator& operator++()
    {
	h = h->next;
	return *this;
    };

    dl_list_const_iterator operator++(int)
    {
	dl_list_const_iterator tmp(this);
	h = h->next;
	return tmp;
    };
    
    bool operator==(const dl_list_const_iterator &x) { return h == x.h; };
    bool operator!=(const dl_list_const_iterator &x) { return h != x.h; };
    
    const T& operator*() { return *(const T*)list_entry(h, T, le); };
};

// ----------------------




// ----------------------


