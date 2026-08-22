type ski = S | K | I | Ptr of node | PLUS | MINUS
and func= SKI_func of ski
and arguement= SKI_arg of ski | Literal of int (*modify later to add more datatypes*)
and node={func:func;arguement:arguement}


let ski_graph = 
    let rec node0 = {func=SKI_func (Ptr node1); arguement= SKI_arg (Ptr node6)}
    and node1=   {func= SKI_func (Ptr node2 ); arguement= SKI_arg I } 
    and node2=   {func= SKI_func S ;arguement= SKI_arg (Ptr node3)}
    and node3=   {func= SKI_func (Ptr node4); arguement=SKI_arg I} 
    and node4=   {func= SKI_func S;arguement= SKI_arg (Ptr node5)}
    and node5=   {func= SKI_func K;arguement= SKI_arg PLUS}
    and node6=   {func= SKI_func (Ptr node7); arguement= Literal 23} 
    and node7=   {func= SKI_func PLUS ; arguement = Literal 100 }
    in node0

let traversed = []

let rec traversal current traversed = 
match current.func with 
(Ptr next) -> traversal next traversed::current
| S -> 
| K -> 
| I -> current.arguement 