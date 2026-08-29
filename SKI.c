//print function for spinestack
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
    } value;
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
    return t;
}

Term make_lit(int n){
    Term t;
    t.kind=KIND_LITERAL;
    t.value.literal=n;
    return t;
}

Node * make_node (Term f,Term a){
    Node * n= (Node *) malloc(sizeof(Node));
    n->func=f;
    n->arguement=a;
    return n;
}

Node * traversal  (Node *current, SpineStack *traversed){
    if (!current)
        return NULL;
    switch(current->func.kind){
        case KIND_PTR:{

            Node *next=current->func.value.ptr;
            SpineStack stack_entry;
            stack_entry.node=current;
            stack_entry.next=traversed;

            Node * retnode= traversal(next, &stack_entry);
            //add safegaurd or whatever the AI said
            current->func.kind=KIND_PTR;
            current->func.value.ptr= retnode;
            return current;
        }
        case  KIND_I:{
            if(current->arguement.kind==KIND_PTR){
                Node * next_node = current->arg.value.ptr;
                current->func=next_node->func;
                current->arguement=next_node->arguement;
                return traversal(current, traversed);
            }
        return current;

        }
    case KIND_S:
    case KIND_K:{
        if(traversed!=NULL){
            Node *parent = traversed->node;
            Term x= current->arguement;
            if(x.kind== KIND_PTR){
                Node * x_node=x.value.ptr;
                parent->func=x_node->func;
                parent->arguement=x_node->arguement;
            } else [
                parent->func=make_prim(KIND_I);
                parent->arguement=x;
            ]

        }
    }
        
    case KIND_PLUS:
    case KIND_MINUS:

    default:
        return current;
    }
}


void main(){

Node* node7 = make_node(make_prim(KIND_PLUS), make_lit(100));
Node* node6 = make_node(make_ptr(node7), make_lit(23));
Node* node5 = make_node(make_prim(KIND_K), make_prim(KIND_PLUS));
Node* node4 = make_node(make_prim(KIND_S), make_ptr(node5));
Node* node3 = make_node(make_ptr(node4), make_prim(KIND_I));
Node* node2 = make_node(make_prim(KIND_S), make_ptr(node3));
Node* node1 = make_node(make_ptr(node2), make_prim(KIND_I));
Node* node0 = make_node(make_ptr(node1), make_ptr(node6));

}