type numeric_operator =
  | Plus
  | Minus
  | Times
  | Div

type logic_operator =
  | Less
  | Greater
  | Equal
  | And
  | Or

type bitwise_operator =
  | LShift
  | RShift
  | BitAnd
  | BitOr

type application =
  | Latoms of atom list

and atom =
  | Var of string
  | Lit of int
  | Aexpr of expr
  | Lam of {
      var : string;
      body : expr;
    }

and cond =
  | Condition of {
      clexpr : expr;
      op : logic_operator;
      crexpr : expr;
    }

and expr =
  | App of application
  | NumericBinOP of {
      lexpr : expr;
      op : numeric_operator;
      rexpr : expr;
    }
  | CondExpr of cond
  | Bwise of {
      blexpr : expr;
      op : bitwise_operator;
      brexpr : expr;
    }
  | Not of expr
  | If of {
      condition : cond;
      thn : expr;
      els : expr;
    }
  | Let of {
      binding : string;
      params : string list;
      eqexpe : expr;
      inexpr : expr;
    }
  | LetRec of {
      binding : string;
      params : string list;
      eqexpe : expr;
      inexpr : expr;
    }
