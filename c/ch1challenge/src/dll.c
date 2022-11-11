#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

typedef struct DLLNode { 
    struct DLLNode *next;
    struct DLLNode *prev;
    const char *val;
} DLLNode;

typedef struct DLL {
    DLLNode* first;
    DLLNode* last;
} DLL;

// invariant: first->prev = NULL and last->next = NULL
// list looks like
// [x] <- [first] <-> [..] <-> [..] <-> ... <-> [..] <-> [last] -> [x]

DLLNode *newDLLNode() {
    DLLNode *node = (DLLNode*) malloc(sizeof(DLLNode));
    assert(node);
    node->next = NULL;
    node->prev = NULL;
    node->val = NULL;

    return node;
}

void setVal(DLLNode *node, const char *val) {
    // copy the string
    unsigned long len = strlen(val);
    const char *copy = strndup(val, len);
    assert(copy);

    node->val = copy;
}

void delVal(DLLNode *node) {
    free((void *)node->val);
}

void setNext(DLLNode *prev, DLLNode *next) {
    prev->next = next;
    next->prev = prev;
}

void delNode(DLLNode *node) {
    if (node->prev) node->prev = node->next;
    if (node->next) node->next = node->prev;
    delVal(node);
    free((void *) node);
}

DLL *newDLL() {
    DLL *dll = (DLL*) malloc(sizeof(DLL));
    assert(dll);
    dll->first = NULL;
    dll->last = NULL;
    return dll;
}

DLLNode *insertLast(DLL *dll, const char *val) {
    // make the new node
    DLLNode *node = newDLLNode();
    setVal(node, val);
    // if list is empty make it first node
    if (!dll->first) {
        // node will be first and last
        dll->first = node;
        node->prev = NULL;
        dll->last = node;
        node->next = NULL;
        return node;
    }
    // otherwise last is not null
    assert(dll->last && dll->last->val && !dll->last->next);
    setNext(dll->last, node);
    dll->last = node;
    node->next = NULL;
    return node;
}

DLLNode *removeLast(DLL *dll) {
    // empty list
    if (!dll->last) return NULL;
    // one element list
    else if (dll->last == dll->first) {
        delNode(dll->last);
        dll->last = NULL;
        dll->first = NULL;
        return NULL;
    }

    // otherwise delete last
    DLLNode *newLast = dll->last->prev;
    delNode(dll->last);
    dll->last = newLast;
    newLast->next = NULL;
    return newLast;
}

DLLNode *nth(DLL *dll, int n) {
    DLLNode *walker = dll->first;
    for (int steps = 0; walker && steps < n; ++steps) {
        walker = walker->next;
    }

    return walker;
}

DLLNode *find(DLL *dll, const char* val) {
    DLLNode *walker = dll->first;
    while(walker && strcmp(walker->val, val) != 0) {
        walker = walker->next;
    }
    
    assert(!walker || strcmp(walker->val, val) == 0);
    return walker;
}

void removeAll(DLL *dll) {
    while (dll->last) {
        removeLast(dll);
    }
}

void delDLL(DLL *dll) {
    removeAll(dll);
    free(dll);
}

void testEmpty() {
    DLL *dll = newDLL();
    assert(!dll->first && !dll->last);
    delDLL(dll);
}

void checkNonEmptyInvariants(const DLL *dll) {
    assert(dll->first && dll->first->val && !dll->first->prev);
    assert(dll->last && dll->last->val && !dll->last->next);
}

void testOneElt() {
    DLL *dll = newDLL();
    const char *val = "one";
    DLLNode *last = insertLast(dll, val);
    checkNonEmptyInvariants(dll);
    assert(dll->first == dll->last);
    assert(strcmp(dll->first->val, val) == 0);
    delDLL(dll);
}

void testTwoElt() {
    DLL *dll = newDLL();
    const char *one = "one";
    const char *two = "two";
    DLLNode *last = insertLast(dll, one);
    last = insertLast(dll, two);
    checkNonEmptyInvariants(dll);
    assert(dll->first->next == dll->last && dll->last->prev == dll->first);
    assert(strcmp(dll->first->val, one) == 0);
    assert(strcmp(dll->last->val, two) == 0);

    removeLast(dll);
    checkNonEmptyInvariants(dll);
    assert(dll->first == dll->last);
    assert(strcmp(dll->first->val, one) == 0);
    delDLL(dll);
}

void testMultipleElt() {
    DLL *dll = newDLL();
    const char *vals[4] = {
        "one", "two", "three", "four"
    };
    DLLNode *last = insertLast(dll, vals[0]);
    last = insertLast(dll, vals[1]);
    last = insertLast(dll, vals[2]);
    last = insertLast(dll, vals[3]);
    checkNonEmptyInvariants(dll);
    assert(dll->first == nth(dll, 0));
    assert(dll->last == nth(dll, 3));
    for (int i = 0; i < 4; ++i) {
        DLLNode *ithNode = nth(dll, i);
        assert(strcmp(ithNode->val, vals[i]) == 0);
        assert(find(dll, vals[i]) == ithNode);
    }
    assert(!find(dll, "five"));
    assert(!find(dll, "zero"));
    delDLL(dll);
}

int main() {
    printf("Checking testEmpty(): ");
    testEmpty();
    printf("good\n");
    printf("Checking testOneElt(): ");
    testOneElt();
    printf("good\n");
    printf("Checking testTwoElt(): ");
    testTwoElt();
    printf("good\n");
    printf("Checking testMultipleElt(): ");
    testMultipleElt();
    printf("good\n");
}
