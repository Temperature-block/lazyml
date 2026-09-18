import sys
import ctypes
from parser import parse_file
from bracker_abs import translate, build_c_term, lib


def run_file(path):
    print("1-3. Parsing, checking, desugaring...")
    surface_tree, lambda_program = parse_file(path)
    print(f"   Surface AST: {surface_tree}")
    print(f"   Lambda AST:  {lambda_program}\n")

    print("4. Bracket-abstracting to SKI...")
    ski_ast = translate(lambda_program)
    print(f"   Compiled SKI AST: {ski_ast}\n")

    print("5. Building C graph in memory...")
    root_term = build_c_term(ski_ast)
    root_node_ptr = root_term.value.ptr

    debug_ptr = ctypes.c_void_p.in_dll(lib, "debug_root")
    debug_ptr.value = ctypes.cast(root_node_ptr, ctypes.c_void_p).value

    print("6. Executing in C engine...")
    lib.print_graph(b"Initial Graph",1)
    lib.traversal(root_node_ptr, None,sys.argv[2])
    lib.print_graph(b"final Graph",1)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python main.py <source_file>")
        sys.exit(1)
    run_file(sys.argv[1])
