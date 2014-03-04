#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct list_node {
    void *data;
    struct list_node *prev, *next;
};

void init_list(struct list_node *head);
#define init_list(list_head)    (list_head) = (struct list_node *)malloc(sizeof(struct list_node)); \
                                (list_head)->prev = NULL; (list_head)->next = NULL;
#define add_data_tail(data, type, list_head)    (data) *node = (struct list_node *)malloc(sizeof((type))); \
                                                    node->next = (list_head); \
                                                    node->prev = (list_head)->prev; \
                                                    (list_head)->prev->next = node; \
                                                    (list_head)->prev = node;
#define list_for_each(entry, list_head)         for ((entry)=(list_head)->next; (entry)!=(list_head); (entry)=(entry)->next)

#endif
