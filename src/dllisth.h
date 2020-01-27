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


#ifndef likely
#if __GNUC__
#  define likely(x)       __builtin_expect((x),1)
#  define unlikely(x)     __builtin_expect((x),0)
#else
#  define likely(x)       x
#  define unlikely(x)     x
#endif
#endif // likely
// ----------------------

struct list_head {
    struct list_head *next, *prev;

    void init(void) { next = prev = this; };

};

struct list_head_c : public list_head
{
    list_head_c() { init(); };
};


typedef struct list_head list_head_t;

// ----------------------


/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
inline void __list_add(struct list_head * enew,
	struct list_head * prev,
	struct list_head * next)
{
	next->prev = enew;
	enew->next = next;
	enew->prev = prev;
	prev->next = enew;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
inline void list_add(struct list_head *enew,struct list_head *head){
	__list_add(enew, head, head->next);
}


/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
inline void list_add_tail(struct list_head *enew, struct list_head *head){
	__list_add(enew, head->prev, head);
};

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
inline void __list_del(struct list_head * prev, struct list_head * next){
	next->prev = prev;
	prev->next = next;
};

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is in an undefined state.
 */
inline void list_del(struct list_head *entry){
	__list_del(entry->prev, entry->next);
};

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
inline void list_del_init(struct list_head *entry){
	__list_del(entry->prev, entry->next);
	entry->init();
//	DL_INIT_LIST_HEAD(entry); 
};


/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
inline bool list_empty(const struct list_head *head){
	return head->next == head;
};



inline void __list_splice(const struct list_head *list,
				 struct list_head *prev,
				 struct list_head *next)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
inline void list_splice(const struct list_head *list,
				struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head, head->next);
}

/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
inline void list_splice_tail(struct list_head *list,
				struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head->prev, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
inline void list_splice_init(struct list_head *list,
				    struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
		list->init();
	}
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
inline void list_splice_tail_init(struct list_head *list,
					 struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head->prev, head);
		list->init();
	}
}




/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
inline void list_move(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add(list, head);
};

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
inline void list_move_tail(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add_tail(list, head);
};


// ----------------------
// list_head wrapper
class dl_list : public list_head_c
{
public:

    dl_list() {};
    ~dl_list() { del_no_init(); };

    void add(list_head *enew) { list_add(enew, this); };
    void add(list_head &n) { add(&n); };

    void add_tail(list_head *enew) { list_add_tail(enew, this); };
    void add_tail(list_head &n) { add_tail(&n); };


    void del(void) { list_del_init(this); };

    void del_no_init(void) { list_del(this); };

    bool empty(void)const { return list_empty(this); };
    operator bool (void) const { return !empty(); };


    void splice(list_head *new_list) { list_splice_init(new_list, this); };
    void splice(list_head &new_list) { splice(&new_list); };

    void splice_no_init(list_head *new_list) { list_splice(new_list, this); };
    void splice_no_init(list_head &new_list) { splice_no_init(&new_list); };
    
    void splice_tail(list_head *new_list) { list_splice_tail_init(new_list, this); };
    void splice_tail(list_head &new_list) { splice_tail(&new_list); };

    void splice_tail_no_init(list_head *new_list) { list_splice_tail(new_list, this); };
    void splice_no_tail_init(list_head &new_list) { splice_tail_no_init(&new_list); };

};

// ----------------------


// ----------------------


