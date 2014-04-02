#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct list_node {
    struct list_node *prev, *next;
};

#define init_list(head)                         head->prev = head; head->next = head;
#define add_data_tail(data, list_head)          if ((list_head)->prev != (list_head)) { \
                                                    (data)->node.next = (list_head); \
                                                    (data)->node.prev = (list_head)->prev; \
                                                    (list_head)->prev->next = &((data)->node); \
                                                    (list_head)->prev = &((data)->node); \
                                                } else { \
                                                    (data)->node.next = (list_head); \
                                                    (data)->node.prev = (list_head); \
                                                    (list_head)->prev->next = &((data)->node); \
                                                    (list_head)->prev = &((data)->node); \
                                                }
#define add_data_before(data, entry, list_head) (data)->node.next = (entry); \
                                                (data)->node.prev = (entry)->prev; \
                                                (entry)->prev->next = &((data)->node); \
                                                (entry)->prev = &((data)->node);
#define add_data_after(data, entry, list_head)  (data)->node.next = (entry)->next; \
                                                (data)->node.prev = (entry); \
                                                (entry)->next->prev = &((data)->node); \
                                                (entry)->next = &((data)->node);
#define list_for_each(entry, list_head)         for ((entry)=(list_head)->next; (entry)!=(list_head); (entry)=(entry)->next)
#define get_data(entry, type)                   (type *)((char *)entry - (sizeof(type) - sizeof(struct list_node)));
#endif
