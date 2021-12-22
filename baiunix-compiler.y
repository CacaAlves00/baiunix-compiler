%{
    #include "baiunix-compiler.h"
%}

%union
{
    struct ast *a;
    double d;
    struct symbol *s;       /* qual símbolo ? */
    struct symlist *sl;     
    int fn;                 /* qual função ? */
};

/* declaração de tokens */
%token <d> NUMBER;
%token <s> NAME;
%token <fn> FUNC;
%token EOL;
%token AND OR

%token IF THEN ELSE WHILE DO FOR LET

%nonassoc <fn> CMP
%right '='
%left '+' '-'
%left '*' '/'
%left AND OR

%type <a> exp stmt list explist cmp
%type <sl> symlist

%start calclist

%%

stmt: IF exp THEN list {
    $$ = newflow('I', $2, $4, NULL, NULL);
}
| IF exp THEN list ELSE list {
    $$ = newflow('I', $2, $4, $6, NULL);
}
| WHILE exp DO list {
    $$ = newflow('W', $2, $4, NULL, NULL);
}
| FOR '(' exp ';' exp ';' exp ')' list {
    $$ = newflow('O', $3, $5, $7, $9);
} 
| exp
;

list: /* vazio */ {
    $$ = NULL;
}
| stmt ';' list {
    if ($3 == NULL) 
        $$ = $1;
    else
        $$ = newast('L', $1, $3);
}
;

exp: cmp 
| exp '+' exp {
    $$ = newast('+', $1, $3);
}
| exp '-' exp {
    $$ = newast('-', $1, $3);
}
| exp '*' exp {
    $$ = newast('*', $1, $3);
}
| exp '/' exp {
    $$ = newast('/', $1, $3);
}
| '('exp')' {
    $$ = $2;
}
| NUMBER {
    $$ = newnum($1);
}
| NAME {
    $$ = newref($1);
}
| NAME '=' exp {
    $$ = newasgn($1, $3);
}
| FUNC '(' explist ')' {
    $$ = newfunc($1, $3);
}
| NAME '(' explist ')' {
    $$ = newcall($1, $3);
}
;

cmp: exp CMP exp {
    $$ = newcmp($2, $1, $3, NULL, 0);
} 
|  exp CMP exp AND cmp {
    $$ = newcmp($2, $1, $3, $5, 1);
} 
| exp CMP exp OR cmp {
    $$ = newcmp($2, $1, $3, $5, 2);
}
;

explist: exp
| exp ',' explist {
    $$ = newast('L', $1, $3);
}
;

symlist: NAME {
    $$ = newsymlist($1, NULL);
}
| NAME ',' symlist {
    $$ = newsymlist($1, $3);
}
;

calclist: /* vazio */
| calclist stmt EOL {
    eval($2);
    treefree($2);
}
| calclist LET NAME '(' symlist ')' '=' list EOL {
    dodef($3, $5, $8);
}
| calclist error EOL {
    yyerrok;
}
;