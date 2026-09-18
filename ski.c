#include <stdio.h>
#include <stdlib.h>
typedef enum {
    KIND_S,
    KIND_K,
    KIND_I,
    KIND_PTR,
    KIND_PLUS,
    KIND_MINUS,
    KIND_DIV,
    KIND_MUL,
    KIND_EQ,
    KIND_LITERAL
} Kind;

struct Node;

typedef struct Term{
    Kind kind;
    union {
        struct Node *ptr;
        int literal;
    }value;
}Term;

typedef struct Node {
    Term func;
    Term arguement;
}Node;

typedef struct SpineStack{
    Node *node;
    struct SpineStack *next;
} SpineStack;

Node *debug_root = NULL;

void print_node(Node *n);

void print_term(Term t) {
    switch(t.kind) {
        case KIND_S:       printf("S"); break;
        case KIND_K:       printf("K"); break;
        case KIND_I:       printf("I"); break;
        case KIND_PLUS:    printf("+"); break;
        case KIND_MINUS:   printf("-"); break;
        case KIND_MUL:     printf("*"); break;
        case KIND_LITERAL: printf("%d", t.value.literal); break;
        case KIND_PTR:     print_node(t.value.ptr); break;
        case KIND_EQ:      printf("="); break;
    }
}

void print_node(Node *n) {
    if (!n) {
        printf("NULL");
        return;
    }
    printf("(");
    print_term(n->func);
    printf(" ");
    print_term(n->arguement);
    printf(")");
}

void print_graph(const char *step_name,int debug_enabled) {
    if(debug_enabled){
       printf("%s: ", step_name);
       print_node(debug_root);
       printf("\n");
       }
}


Node * traversal (Node *current, SpineStack *traversed,int debug);

int evaluate_to_int(Term t){
    if (t.kind==KIND_LITERAL){
        return t.value.literal;
    }
    if(t.kind==KIND_PTR){ //?
        Node *res=traversal(t.value.ptr, NULL, 0);
         if (res->func.kind == KIND_I && res->arguement.kind == KIND_LITERAL) {
            return res->arguement.value.literal;
        }
    }
    printf("Error: Could not evaluate branch to an integer.\n");
    exit(1);
}

Term make_prim(Kind k) {
    Term t;
    t.kind = k;
    return t;
}

Term make_ptr(Node *n) {
    Term t;
    t.kind = KIND_PTR;
    t.value.ptr = n;
    return t;
}

Term make_lit(int n) {
    Term t;
    t.kind = KIND_LITERAL;
    t.value.literal = n;
    return t;
}

Node * make_node(Term f, Term a){
    Node *n=(Node *)malloc(sizeof(Node));
    n->func=f;
    n->arguement=a;
    return n;
}

Node *traversal(Node *current, SpineStack *traversed,int debug) {
    if (!current)
        return NULL;

    switch(current->func.kind) {
        case KIND_PTR: {
            Node * next= current->func.value.ptr;
            SpineStack stack_entry;
            stack_entry.node=current;
            stack_entry.next=traversed;

            Node *retnode = traversal(next, &stack_entry,debug);

            if (current->func.kind==KIND_PTR && current->func.value.ptr == next){
                current->func.value.ptr=retnode;

             if (retnode->func.kind == KIND_I && retnode->arguement.kind != KIND_PTR) {
                current->func = retnode->arguement;
                print_graph("Indirection removed",debug);
                return traversal(current, traversed, debug);
            }

            return current;
        } else {
            return traversal(current, traversed, debug); 
        }
    }

    case KIND_I: {
        if (current->arguement.kind == KIND_PTR) {
            Node *next_node = current->arguement.value.ptr;
            current->func = next_node->func;
            current->arguement = next_node->arguement;
            print_graph("Rule I fired",debug);
            return traversal(current, traversed, debug);
        }
    return current;
    } 

    case KIND_K: {
        if (traversed != NULL) {
            Node *parent = traversed->node;
            Term x = current->arguement;
            if (x.kind == KIND_PTR) {
                Node *x_node = x.value.ptr;
                parent->func = x_node->func;
                parent->arguement = x_node->arguement;
            } else {
                parent->func = make_prim(KIND_I);
                parent->arguement = x;
            }
            print_graph("Rule K fired",debug);
        }
        return current;
    } 

     case KIND_S: {
        if (traversed != NULL && traversed->next != NULL) {
            Node *parent = traversed->node;
            Node *grandparent = traversed->next->node;
            Term f = current->arguement;
            Term g = parent->arguement;
            Term x = grandparent->arguement;
            Node *left_child = make_node(f, x);
            Node *right_child = make_node(g, x);
            grandparent->func = make_ptr(left_child);
            grandparent->arguement = make_ptr(right_child);
            print_graph("Rule S fired",debug);
        }
        return current;
        }

        case KIND_PLUS: {
            if (traversed != NULL) {
                Node *parent = traversed->node;
                int x_val = evaluate_to_int(current->arguement);
                int y_val = evaluate_to_int(parent->arguement);
                int sum = x_val + y_val; 
                parent->func = make_prim(KIND_I);
                parent->arguement = make_lit(sum);
                print_graph("Rule PLUS fired",debug);
            }
            return current;
        } 
    
        case KIND_MINUS: {
            if (traversed != NULL) {
                Node *parent = traversed->node;
                int x_val = evaluate_to_int(current->arguement);
                int y_val = evaluate_to_int(parent->arguement);
                parent->func = make_prim(KIND_I);
                parent->arguement = make_lit(x_val - y_val);
                print_graph("Rule MINUS fired",debug);
            }
            return current;
        } 

        case KIND_MUL: {
            if (traversed != NULL) {
                Node *parent = traversed->node;
                int x_val = evaluate_to_int(current->arguement);
                int y_val = evaluate_to_int(parent->arguement);
                parent->func = make_prim(KIND_I);
                parent->arguement = make_lit(x_val * y_val);
                print_graph("Rule MUL fired",debug);
            }
            return current;
        }
       case KIND_EQ: {
            if (traversed != NULL) {
               Node *parent = traversed->node;
               int x_val = evaluate_to_int(current->arguement);
               int y_val = evaluate_to_int(parent->arguement);

               if (x_val == y_val) {
                  // TRUE = K
                  parent->func = make_prim(KIND_I);
                  parent->arguement = make_prim(KIND_K);
               } else {
                 // FALSE = K I  (an application, needs a real Node)
                 Node *ki_node = make_node(make_prim(KIND_K), make_prim(KIND_I));
                 parent->func = make_prim(KIND_I);
                 parent->arguement = make_ptr(ki_node);
               }
                 print_graph("Rule EQ fired",debug);
            }
    return current;
}
        default:
        return current;
    }
}

