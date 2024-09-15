#include "list.h"
#include <assert.h>

static bool isInitialized = false;

static Node nodeArr[LIST_MAX_NUM_NODES];
static List listArr[LIST_MAX_NUM_HEADS];

static Node* freeNodesHead = nodeArr;
static List* freeListsHead = listArr;

static void setup() {
    Node* node = freeNodesHead;
    for (size_t i = 0; i < LIST_MAX_NUM_NODES-1; i++)
    {
        node->next = node+1;
        node = node->next;
    }
    node->next = NULL;

    List* list = freeListsHead;
    for (size_t i = 0; i < LIST_MAX_NUM_HEADS-1; i++)
    {
        // list->current is used to store the location of the next list when not in use
        list->current = (Node*)(list+1);
        list = list+1;
    }
    list->current = NULL;

    isInitialized = true;
}

static Node* Node_create(Node* pPrev, Node* pNext, void* pItem) {
    if(freeNodesHead == NULL) {
        return NULL;
    }

    Node* ret = freeNodesHead;
    freeNodesHead = freeNodesHead->next;
    ret->prev = pPrev;
    ret->next = pNext;
    ret->data = pItem;

    return ret;
}

static void Node_free(Node* pNode) {
    pNode -> next = freeNodesHead;
    freeNodesHead = pNode;
}

List *List_create()
{
    if(isInitialized == false) {
        setup();
    }

    if(freeListsHead == NULL) {
        return NULL;
    }
    List* list = freeListsHead;
    freeListsHead = (List*)freeListsHead->current; //it is used to store the addres of the next list when not in use
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->size = 0;
    list->state = LIST_OOB_END;
    return list;
}

int List_count(List *pList)
{
    assert(pList != NULL);
    return pList->size;
}

void *List_first(List *pList)
{
    assert(pList != NULL);
    if(pList->size == 0){
        return NULL;
    }
    pList->current = pList->head;
    return pList->head->data;
}

void *List_last(List *pList)
{
    assert(pList != NULL);
    if(pList->size == 0){
        return NULL;
    }
    pList->current = pList->tail;
    return pList->tail->data;
}

void *List_next(List *pList)
{
    assert(pList != NULL);
    if(pList->size == 0){
        if(pList->state == LIST_OOB_START) {
            pList->state = LIST_OOB_END;
        }
        return NULL;
    }
    if(pList->current == NULL) {
        if(pList->state == LIST_OOB_START) {
            pList->current = pList->head;
        }
    }
    else if(pList->current == pList->tail) {
        pList->current = NULL;
        pList->state = LIST_OOB_END;
    }
    else {
        pList->current = pList->current->next;
    }

    if(pList->current == NULL) {
        return NULL;
    }
    return pList->current->data;
}

void *List_prev(List *pList)
{
    assert(pList != NULL);
    if(pList->size == 0){
        if(pList->state == LIST_OOB_END) {
            pList->state = LIST_OOB_START;
        }
        return NULL;
    }
    if(pList->current == NULL) {
        if(pList->state == LIST_OOB_END) {
            pList->current = pList->tail;
        }
    }
    else if(pList->current == pList->head) {
        pList->current = NULL;
        pList->state = LIST_OOB_START;
    }
    else {
        pList->current = pList->current->prev;
    }

    if(pList->current == NULL) {
        return NULL;
    }
    return pList->current->data;
}

void *List_curr(List *pList)
{
    assert(pList != NULL);
    if(pList->current == NULL){
        return NULL;
    }
    return pList->current->data;
}

int List_insert_after(List *pList, void *pItem)
{
    assert(pList != NULL);
    if(pList->current == NULL) {
        if(pList->state == LIST_OOB_START) {
            return List_prepend(pList, pItem);
        }
        if(pList->state == LIST_OOB_END) {
            return List_append(pList, pItem);
        }
    }
    if(pList->current == pList->tail) {
        return List_append(pList, pItem);
    }

    Node* pNode = Node_create(pList->current, pList->current->next, pItem);
    if(pNode == NULL) {
        return LIST_FAIL;
    }
    pList->current->next->prev = pNode;
    pList->current->next = pNode;
    pList->current = pNode;
    pList->size++;

    return LIST_SUCCESS;
}

int List_insert_before(List *pList, void *pItem)
{
    assert(pList != NULL);
    if(pList->current == NULL) {
        if(pList->state == LIST_OOB_START) {
            return List_prepend(pList, pItem);
        }
        if(pList->state == LIST_OOB_END) {
            return List_append(pList, pItem);
        }
    }
    if(pList->current == pList->head) {
        return List_prepend(pList, pItem);
    }

    Node* pNode = Node_create(pList->current->prev, pList->current, pItem);
    if(pNode == NULL) {
        return LIST_FAIL;
    }
    pList->current->prev->next = pNode;
    pList->current->prev = pNode;
    pList->current = pNode;
    pList->size++;

    return LIST_SUCCESS;
}

int List_append(List *pList, void *pItem)
{
    assert(pList != NULL);

    Node* pNode = Node_create(pList->tail, NULL, pItem);
    if(pNode == NULL) {
        return LIST_FAIL;
    }
    if(pList->size == 0) {
        pList->head = pNode;
    }
    else {
        pList->tail->next = pNode;
    }
    pList->tail = pNode;
    pList->current = pNode;
    pList->size++;

    return LIST_SUCCESS;
}

int List_prepend(List *pList, void *pItem)
{
    assert(pList != NULL);

    Node* pNode = Node_create(NULL, pList->head, pItem);
    if(pNode == NULL) {
        return LIST_FAIL;
    }
    if(pList->size == 0) {
        pList->tail = pNode;
    }
    else {
        pList->head->prev = pNode;
    }
    pList->head = pNode;
    pList->current = pNode;
    pList->size++;

    return LIST_SUCCESS;
}

void *List_remove(List *pList)
{
    assert(pList != NULL);

    if(pList->current == NULL) {
        return NULL;
    }

    Node* pNode = pList->current;
    void* ret = pNode->data;

    if(pList->size == 1) {
        pList->head = NULL;
        pList->tail = NULL;
        pList->current = NULL;
        pList->state = LIST_OOB_END;
    }
    else if(pNode == pList->head) {
        pList->head = pList->head->next;
        pList->current = pList->head;
        pList->current->prev = NULL;
    }
    else if(pNode == pList->tail) {
        pList->tail = pList->tail->prev;
        pList->current = NULL;
        pList->state = LIST_OOB_END;
        pList->tail->next = NULL;
    }
    else {
        pList->current->prev->next = pList->current->next;
        pList->current->next->prev = pList->current->prev;
        pList->current = pList->current->next;
    }

    Node_free(pNode);
    pList->size--;

    return ret;
}

void *List_trim(List *pList)
{
    assert(pList != NULL);
    if(pList->size == 0) {
        return NULL;
    }
    

    Node* pNode = pList->tail;
    void* ret = pNode->data;

    if(pList->size == 1) {
        pList->head = NULL;
        pList->tail = NULL;
        pList->current = NULL;
        pList->state = LIST_OOB_START;
    }
    else {
        pList->tail = pList->tail->prev;
        pList->tail->next = NULL;
        pList->current = pList->tail;
    }

    Node_free(pNode);
    pList->size--;

    return ret;
}

void List_concat(List *pList1, List *pList2)
{
    assert(pList1 != NULL && pList2 != NULL);
    if(pList1->size == 0) {
        pList1->head = pList2->head;
    }
    else {
        pList1->tail->next = pList2->head;
    }

    if(pList2->size != 0) {
        pList1->tail = pList2->tail;
    }

    pList1->size += pList2->size;

    pList2->current = (Node*)freeListsHead;
    freeListsHead = pList2;
}

void List_free(List *pList, FREE_FN pItemFreeFn)
{
    pList->current = pList->head;
    while(pList->current != NULL) {
        Node* node = pList->current;
        if(node->data != NULL) {
            (*pItemFreeFn)(node->data);
        }
        pList->current = pList->current->next;
        Node_free(node);
    }

    pList->current = (Node*)freeListsHead;
    freeListsHead = pList;
}

void *List_search(List *pList, COMPARATOR_FN pComparator, void *pComparisonArg)
{
    assert(pList != NULL);
    if(pList->current == NULL && pList->state == LIST_OOB_START) {
        pList->current = pList->head;
    }
    while(pList->current != NULL) {
        if((*pComparator)(pList->current->data, pComparisonArg) == 1) {
            return pList->current->data;
        }
        pList->current = pList->current->next;
    }
    pList->state = LIST_OOB_END;
    return NULL;
}
