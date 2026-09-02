{
open  Parser
}

let letrec = "letrec"
let let_kw = "let"
let in_kw = "in"
let assgn = ":="

let binding = ['a'-'z' 'A'-'Z' '_'] ['a'-'z' 'A'-'Z' '0'-'9' '_']*
let num = ['0'-'9']+

let numplus = "+"
let numminus = "-"
let nummul = "*"
let numdiv = "/"

let numlt = "<"
let numgt = ">"
let numeq = "="

let logand = "AND"
let logor = "OR"

let bsl = "<<"
let bsr = ">>"
let ba = "&"
let bor = "||"
let bnot = "!"

let popen = "("
let pclose = ")"

let wsp = [' ' '\t' '\n' '\r']+

rule read = 
  parse
    | wsp { read lexbuf }
    | letrec { LETREC }
    | let_kw { LET }
    | in_kw { IN }
    | logand { AND }
    | logor { OR }
    | assgn { ASGN }
    | binding { BINDING (Lexing.exeme lexbuf) }
    | num { INT (int_of_string (Lexing.lexeme lexbuf)) }
    | numplus { NUMPLUS }
    | numminus { NUMMINUS }
    | nummul { NUMMUL }
    | numdiv { NUMDIV }
    | bsl { BSL }
    | bsr { BSR }
    | ba { BAND }
    | bor { BOR }
    | bnot { BNOT }
    | popen { POPEN }
    | pclosen { PCLOSE }
    | eof { EOF }
