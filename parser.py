
import sys

'''
<program> ::= <expr>

<expr> ::= <int>
         | <ident>
         | "(" "lambda" "(" <ident> ")" <expr> ")"
         | "(" "let" "(" <ident> <expr> ")" <expr> ")"
         | "(" "letrec" "(" <ident> <expr> ")" <expr> ")"
         | "(" "if" <expr> <expr> <expr> ")"
         | "(" "+" <expr> <expr> ")"
         | "(" "-" <expr> <expr> ")"
         | "(" "*" <expr> <expr> ")"
         | "(" "=" <expr> <expr> ")"
         | "(" "<" <expr> <expr> ")"
         | "(" <expr> <expr>+ ")"              (* application, catch-all *)

<ident> ::= <letter> (<letter> | <digit> | "_")*
<int>   ::= "-"? <digit>+
<letter> ::= "a".."z" | "A".."Z"
<digit>  ::= "0".."9"
'''

KEYWORDS = {"lambda", "let", "letrec", "if", "+", "-", "*", "=", "<"}


def gettok(prgrm):
    x = prgrm.replace("(", " ( ")
    x = x.replace(")", " ) ")
    x = x.split()
    return x


def atomize(token):
    try:
        return int(token)
    except Exception:
        return token


def parse(tokens):
    token = tokens.pop(0)
    if token == "(":
        node = []
        while tokens[0] != ")":
            node.append(parse(tokens))
        tokens.pop(0)
        return node
    elif token == ")":
        raise SyntaxError("unexpected )")
    else:
        return atomize(token)


def check(node):
    if isinstance(node, int):
        return
    if isinstance(node, str):
        if node in KEYWORDS:
            raise SyntaxError(f"keyword '{node}' used as bare atom")
        return
    if not isinstance(node, list) or len(node) == 0:
        raise SyntaxError(f"empty or malformed form: {node}")

    head = node[0]

    if head == "lambda":
        if len(node) != 3:
            raise SyntaxError(f"lambda needs (lambda (ident) expr), got {node}")
        params, body = node[1], node[2]
        if not isinstance(params, list) or len(params) != 1 or not isinstance(params[0], str):
            raise SyntaxError(f"lambda param must be (ident), got {params}")
        check(body)

    elif head in ("let", "letrec"):
        if len(node) != 3:
            raise SyntaxError(f"{head} needs (name value) body, got {node}")
        binding, body = node[1], node[2]
        if not isinstance(binding, list) or len(binding) != 2 or not isinstance(binding[0], str):
            raise SyntaxError(f"{head} binding must be (ident expr), got {binding}")
        check(binding[1])
        check(body)

    elif head == "if":
        if len(node) != 4:
            raise SyntaxError(f"if needs 3 exprs, got {node}")
        for sub in node[1:]:
            check(sub)

    elif head in ("+", "-", "*", "=", "<"):
        if len(node) != 3:
            raise SyntaxError(f"{head} needs exactly 2 args, got {node}")
        check(node[1]); check(node[2])

    else:
        if len(node) < 2:
            raise SyntaxError(f"application needs at least one argument: {node}")
        for sub in node:
            check(sub)


def desugar(node):
    if isinstance(node, int):
        return node

    if isinstance(node, str):
        return node

    head = node[0]

    if head == "lambda":
        param = node[1][0]
        body = desugar(node[2])
        return ["lambda", [param], body]

    if head == "let":
        name = node[1][0]
        value = desugar(node[1][1])
        body = desugar(node[2])
        return [["lambda", [name], body], value]

    if head == "letrec":
        name = node[1][0]
        value = desugar(node[1][1])
        body = desugar(node[2])
        # Y = \f. (\x. f (\v. ((x x) v))) (\x. f (\v. ((x x) v)))
        inner_term = ["lambda", ["x"],
                      ["f", ["lambda", ["v"], [["x", "x"], "v"]]]]
        y_combinator = ["lambda", ["f"], [inner_term, inner_term]]
        rec_value = [y_combinator, ["lambda", [name], value]]
        return [["lambda", [name], body], rec_value]

    if head == "if":
       cond = desugar(node[1])
       then_b = desugar(node[2])
       else_b = desugar(node[3])
       return [[cond, then_b], else_b]

    if head in ("+", "-", "*", "=", "<"):
        left = desugar(node[1])
        right = desugar(node[2])
        return [[head, left], right]

    fn = desugar(node[0])
    for arg in node[1:]:
        fn = [fn, desugar(arg)]
    return fn


def parse_file(path):
    with open(path) as f:
        source = f.read()
    tokens = gettok(source)
    surface_tree = parse(tokens)
    check(surface_tree)
    lambda_program = desugar(surface_tree)
    return surface_tree, lambda_program


if __name__ == "__main__":
    print(parse(gettok(open(sys.argv[1]).read())))
