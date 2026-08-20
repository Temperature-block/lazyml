type token = LET | LETREC | IN | ASSGN | BINDING of string | NUM of int | NUMPLUS | NUMMINUS | NUMMUL | NUMDIV
             | LOGLT | LOGGT | LOGEQ | LOGAND | LOGOR | BITSHIFTLEFT | BITSHIFTRIGT | BITAND | BITOR | BITNOT
             | POPEN | PCLOSE | WSP

type scanres = FAIL | PASS of (int * token)

let refunc regex = Str.regexp regex

let patterns = [
  "letrec";
  "let";
  "in";
  ":=";
  "[a-zA-Z_][a-zA-Z0-9_]*";
  "[0-9]+";
  "+";
  "-";
  "*";
  "\\";
  "<<";
  ">>";
  "<";
  ">";
  "=";
  "AND";
  "OR";
  "&";
  "||";
  "!";
  "(";
  ")";
  "[ \t\n\r]+"
]

let regexfuncs = let rec patternify lis pats =
                     match pats with
                     [] -> lis
                     | x::rest -> patternify (lis:: (x,(refunc x))) rest
                 in
                 patternify [] patterns

let classifytok inp str = match inp with
  | ("letrec", _) -> LETREC
  | ("let", _) -> LET
  | ("in", _) -> IN
  | (":=", _) -> ASSGN
  | ("[a-zA-Z_][a-zA-Z0-9_]*", _) -> BINDING str
  | ("[0-9]+", _) -> NUM (int_of_string str)
  | ("+", _) ->  NUMPLUS
  | ("-", _) -> NUMMINUS
  | ("*", _) -> NUMMUL
  | ("\\", _) -> NUMDIV
  | ("<<", _) -> BITSHIFTLEFT
  | (">>", _) -> BITSHIFTRIGHT
  | ("<", _) -> LOGLT
  | (">", _) -> LOGGT
  | ("=", _) -> LOGEQ
  | ("AND", _) -> LOGAND
  | ("OR", _) -> LOGOR
  | ("&", _) -> BITAND
  | ("||", _) -> BITOR
  | ("!", _) ->  BITNOT
  | ("(", _) -> POPEN
  | (")", _) -> PCLOSE
  | ("[ \t\n\r]+", _) -> WSP


let rec gettok str pats = match pats with
                          [] -> FAIL
                          | (som,patt) :: rest -> if Str.string_match patt str 0 then
                            let resclass = classifytok (som,patt) (matched_string str)
                            in
                            if resclass <> WSP then PASS (Str.match_end () ,resclass)
let slice_to_end s start_idx =
  let len = String.length s - start_idx in
  String.sub s start_idx len

let matchpats input = let rec scan instr matchlist toklist =
                      match input with
                      "" ->  toklist
                      | strr ->  let res = gettok instr toklist in
                                 if res <> FAIL then let (pos,tok) = res in
                                 let slicedstr = slice_to_end instr in
                                 scan slicedstr matchlist (toklist::tok)
                                 else []
