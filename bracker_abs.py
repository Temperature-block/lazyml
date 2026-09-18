import ctypes
import os

KIND_S, KIND_K, KIND_I, KIND_PTR = 0, 1, 2, 3
KIND_PLUS, KIND_MINUS, KIND_DIV, KIND_MUL, KIND_LITERAL, KIND_EQ = 4, 5, 6, 7, 9, 8

class Node(ctypes.Structure): pass
class TermValue(ctypes.Union): _fields_ = [("ptr", ctypes.POINTER(Node)), ("literal", ctypes.c_int)]
class Term(ctypes.Structure):  _fields_ = [("kind", ctypes.c_int), ("value", TermValue)]
Node._fields_ = [("func", Term), ("arguement", Term)]

lib = ctypes.CDLL(os.path.abspath(f"./libski.so"))

lib.make_prim.argtypes = [ctypes.c_int]
lib.make_prim.restype = Term
lib.make_ptr.argtypes = [ctypes.POINTER(Node)]
lib.make_ptr.restype = Term
lib.make_lit.argtypes = [ctypes.c_int]
lib.make_lit.restype = Term
lib.make_node.argtypes = [Term, Term]
lib.make_node.restype = ctypes.POINTER(Node)
lib.traversal.argtypes = [ctypes.POINTER(Node), ctypes.c_void_p]
lib.traversal.restype = ctypes.POINTER(Node)
lib.print_graph.argtypes = [ctypes.c_char_p]
lib.print_graph.restype = None

def prim(kind): return lib.make_prim(kind)
def lit(val): return lib.make_lit(val)
def ptr(node): return lib.make_ptr(node)
def app(f, a): return lib.make_node(f, a)


def translate(ast):
    if not isinstance(ast, list):
        return ast
    if len(ast) == 3 and ast[0] == 'lambda':
        var_name = ast[1][0]
        body = ast[2]
        return abstract(var_name, translate(body))
    if len(ast) == 2:
        return [translate(ast[0]), translate(ast[1])]
    raise ValueError(f"Invalid AST node: {ast}")

def abstract(var, expr):
    if expr == var:
        return 'I'
    if not isinstance(expr, list):
        return ['K', expr]
    if len(expr) == 2:
        return [['S', abstract(var, expr[0])], abstract(var, expr[1])]
    raise ValueError(f"Invalid expression: {expr}")


def build_c_term(ast):
    if isinstance(ast, list):
        left_term = build_c_term(ast[0])
        right_term = build_c_term(ast[1])
        
        c_node_ptr = app(left_term, right_term)
        return ptr(c_node_ptr)
        
    elif isinstance(ast, int):
        return lit(ast)
        
    elif isinstance(ast, str):
        if ast == 'S': return prim(KIND_S)
        if ast == 'K': return prim(KIND_K)
        if ast == 'I': return prim(KIND_I)
        if ast == '+': return prim(KIND_PLUS)
        if ast == '-': return prim(KIND_MINUS)
        if ast == '*': return prim(KIND_MUL)
        if ast == '/': return prim(KIND_DIV)
        if ast == '=': return prim(KIND_EQ)
        raise ValueError(f"Unknown primitive: {ast}")
        
    raise ValueError(f"Unknown node type: {ast}")


