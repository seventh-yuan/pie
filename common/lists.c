/**
  *******************************************************
  *@file        lists.c
  *@author      Arno
  *@version     V1.0.0
  *@date        2018/4/1
  *@brief       kernel list module.
  *             This is the kernel part of ecode.
  *******************************************************
  */
/* Includes----------------------------------------------*/
#include <common/pie.h>

/*Private variables--------------------------------------*/
/*Public variables---------------------------------------*/
/*Private functions--------------------------------------*/
/*Public functions---------------------------------------*/
  
pie_inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->prev=NULL;
    entry->next=NULL;
}

pie_inline void __list_add(struct list_head *entry,struct list_head *prev, struct list_head *next)
{
    next->prev = entry;
    entry->next = next;
    entry->prev = prev;
    prev->next = entry;
}

void list_add(struct list_head *head, struct list_head *entry)
{
    __list_add(entry, head, head->next);

}

int list_empty(const struct list_head *head)
{
    return (head->next==head);
}