#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(*head));
    if (!head) {
        free(head);
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        q_release_element(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
element_t *new_element(char *s)
{
    element_t *element = malloc(sizeof(*element));
    if (!element) {
        free(element);
        return NULL;
    }
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return NULL;
    }
    return element;
}

bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = new_element(s);
    if (!element)
        return false;
    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = new_element(s);
    if (!element)
        return false;
    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp = list_first_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&tmp->list);
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp = list_last_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&tmp->list);
    return tmp;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    size_t len = 0;
    struct list_head *cur;
    list_for_each (cur, head) {
        ++len;
    }
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    size_t len = q_size(head), count = -1;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        ++count;
        if (count == len / 2) {
            list_del(&entry->list);
            q_release_element(entry);
            break;
        }
    }
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;
    bool flag = 0;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (&safe->list == head) {
            if (flag == 1) {
                list_del(&entry->list);
                q_release_element(entry);
            }
            break;
        }

        if (strcmp(entry->value, safe->value) == 0) {
            list_del(&entry->list);
            q_release_element(entry);
            flag = 1;
        } else if (flag == 1) {
            list_del(&entry->list);
            q_release_element(entry);
            flag = 0;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe, *head_from = head, tmp;
    INIT_LIST_HEAD(&tmp);
    size_t count = 0;
    list_for_each_safe (node, safe, head) {
        ++count;
        if (count == k) {
            list_cut_position(&tmp, head_from, node);
            q_reverse(&tmp);
            list_splice_init(&tmp, head_from);
            head_from = safe->prev;
            count = 0;
        }
    }
    return;
}

/* Sort elements of queue in ascending order */
struct list_head *mergeTwoList(struct list_head *l1, struct list_head *l2)
{
    if (!l2)
        return l1;
    if (!l1)
        return l2;

    element_t *n1 = list_entry(l1, element_t, list);
    element_t *n2 = list_entry(l2, element_t, list);

    if (strcmp(n1->value, n2->value) < 0) {
        l1->next = mergeTwoList(l1->next, l2);
        return l1;
    } else {
        l2->next = mergeTwoList(l1, l2->next);
        return l2;
    }
}

struct list_head *mergeSortList(struct list_head *head)
{
    // merge sort
    if (!head || !head->next)
        return head;

    struct list_head *fast = head->next;
    struct list_head *slow = head;

    // split list
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    // sort each list
    struct list_head *l1 = mergeSortList(head);
    struct list_head *l2 = mergeSortList(fast);

    // merge sorted l1 and sorted l2
    return mergeTwoList(l1, l2);
}

void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = mergeSortList(head->next);

    struct list_head *cur;
    for (cur = head; cur->next; cur = cur->next)
        cur->next->prev = cur;
    cur->next = head;
    head->prev = cur;
    return;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    element_t *maxNode = list_last_entry(head, element_t, list),
              *curNode = NULL;
    int count = 0;
    for (struct list_head *cur = head->prev, *safe = cur->prev; cur != head;
         cur = safe, safe = cur->prev) {
        curNode = list_entry(cur, element_t, list);
        if (strcmp(maxNode->value, curNode->value) > 0) {
            list_del(&curNode->list);
            q_release_element(curNode);
        } else {
            maxNode = curNode;
            ++count;
        }
    }

    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    queue_contex_t *first = list_first_entry(head, queue_contex_t, chain),
                   *entry = NULL;
    list_for_each_entry (entry, head, chain) {
        if (entry->id == first->id) {
            continue;
        }
        list_splice_init(entry->q, first->q);
    }
    q_sort(first->q);
    return q_size(first->q);
}
