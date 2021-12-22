#ifndef BAIUNIX_COMPILER
#define BAIUNIX_COMPILER

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/* declarações para uma calculadora avançada */

/* interface com o lexer */

extern int yylineno; /* do lexer */
extern FILE *yyin;   /* do lexer */
void yyerror(char *s, ...);

/* tabela de símbolos */
struct symbol 
{
    char *name;             /* nome de uma variável */
    double value;       
    struct ast *func;       /* stmt para a função */
    struct symlist *syms;   /* lista de argumentos */
};

/* tab. de símbolos de tamanho fixo */
#define NHASH 9997
struct symbol symtab[NHASH];

struct symbol *lookup(char *);

/* lista de símbolos, para uma lista de argumentos */
struct symlist 
{
    struct symbol *sym;
    struct symlist *next;
};

struct symlist *newsymlist(struct symbol *sym, struct symlist *next);
void symlistfree(struct symlist *sl);

/* tipos de nós
* + - * /
* 0-7 CMP, 04 igual, 02 menor que, 01 maior que
* L expressão ou lista de comandos
* I comando IF
* W comando WHILE
* O comando FOR
* N symbol de referência
* = atribuição
* F chamada de função pré-definida
* C chamada de função definida pelo usuário
*/

enum bifs /* funções pré-definidas */
{
    B_sqrt = 1,
    B_exp,
    B_log,
    B_print
};

/* nós na AST */
/* todos têm o "nodetype" inicial em comum */

struct ast
{
    int nodetype;
    struct ast *l;
    struct ast *r;
};

struct comp
{
    int nodetype;
    struct ast *l;
    struct ast *r;
    int op;
    struct ast *next;
};

struct fncall   /* funções pré-definidas */
{
    int nodetype;   /* tipo F */
    struct ast *l;
    enum bifs functype;
};

struct ufncall  /* funções de usuário */
{
    int nodetype;
    struct ast *l;
    struct symbol *s;
};

struct flow
{
    int nodetype;       /* tipo I ou W ou O*/
    union
    {
        struct ast *cond;   /* condição */
        struct ast *init;   /* declaração de variável para laço for*/        
    };
    union 
    {
        struct ast *tl;         /* ramo "then" ou list "do" */
        struct ast * forcond;   /* condição para laço for */
    };
    union 
    {
        struct ast *el;     /* ramo opcional "else" */
        struct ast *inc;    /* incremento de variável para laço for */
    };
    struct ast *forl;    /* ramo do laço for */
};

struct numval
{
    int nodetype;       /* tipo K */
    double number;
};

struct symref
{
    int nodetype;       /* tipo N */
    struct symbol *s;
};

struct symasgn
{
    int nodetype;       /* tipo = */
    struct symbol *s;
    struct ast *v;      /* valor a ser atribuído */
};

/* construção de uma ast */

struct ast *newast(int nodetype, struct ast *l, struct ast *r); 
struct ast *newcmp(int cmptype, struct ast *l, struct ast *r, struct ast *next, int nextOp);
struct ast *newfunc(int functype, struct ast *l);
struct ast *newcall(struct symbol *s, struct ast *l);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newnum(double d);
struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *tr, struct ast *forl);

/* definição de uma função */
void dodef(struct symbol *name, struct symlist *syms, struct ast *stmts);

/* avaliação de uma AST */
double eval(struct ast *);

/* deletar e liberar uma AST */
void treefree(struct ast *);

#endif