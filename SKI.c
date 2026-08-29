#include <stdio.h>
#include <stdlib.h>

typedef enum {
    KIND_S,
    KIND_K,
    KIND_I,
    KIND_PTR,
    KIND_PLUS,
    KIND_MINUS,
    KIND_LITERAL
} Kind;

struct Node;

typedef struct Term {
    Kind kind;
    union{
        struct Node * ptr;
        int literal;
    }value
} Term;

typedef struct Node {
    Term func;
    Term arguement;
} Node;

typedef struct SpineStack{
    Node *node;
    struct  SpineStack *next;
} SpineStack;

Term make_prim (Kind k){
    Term t;
    t.kind=k;
    return t;
}

Term make_ptr (Node *n){
    Term t;
    t.kind=KIND_PTR;
    t.value.ptr=n;
}

Term make_lit(Node *n){
    Term t;
    t.kind=KIND_LITERAL;
    t.value.literal=n;
}

Node * make_node (Term f,Term a){
    Node * n= (Node *) malloc(sizeof(Node));
    n->func=f;
    n->arguement=a;
    return n;
}

Node * traversal  (Node *current, SpineStack *traversal){
    if (!current)
        return NULL;
    switch(current->func.kind){
        case KIND_PTR;{
            
            Node *next=current->func.value.ptr;
            SpineStack stack_entry;
            SpineStack.node=current;
            SpineStack.next=traversed;

            Node 
        }
        case  KIND_I;{

        }
    }
}


void main(){

}