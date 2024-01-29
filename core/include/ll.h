#ifndef LL_H
#define LL_H

#include <malloc.h>
#include <stdio.h>
#include <type.h>
#include <string.h>

XLll * ll_create(uint size);

int ll_add_member_tail(XLll * ll,void * member,uint member_size);

int ll_add_member_head(XLll * ll,void * member,uint member_size);

int ll_add_ll(XLll * ll1,XLll* ll2);

int ll_insert_member_next(XLll * ll,void * member,uint member_size,uint member_num);

int ll_insert_member_front(XLll * ll,void * member,uint member_size,uint member_num);


int ll_del_member_num(XLll * ll,uint member_num);

int ll_del_member(XLll *ll,XLll_member * member);

int ll_del_member_all(XLll * ll);

void ll_free(XLll * ll);

int ll_del_member_head(XLll * ll);

int ll_del_member_tail(XLll * ll);

void ll_show_member(XLll *ll);

int ll_cpy(XLll * dest,XLll *src);

void * ll_get_member(XLll * ll,uint member_num);

XLll_member * ll_get_member_compare(XLll * ll,uint front_size,uint size,void *compare);

#endif // LL_H
