#ifndef TOKENS_H
#define TOKENS_H

#include <ds/buffer.h>
#include <stddef.h>

/* https://realpython.com/python-keywords/ */
typedef enum {
    T_NULL, /* No token */
    T_ID,   /* my_variable1 */

    /* --- KEYWORDS --- */
    T_AND,      /* and */
    T_ASSERT,   /* assert */
    T_BREAK,    /* break */
    T_CASE,     /* case */
    T_CONTINUE, /* continue */
    T_DEFAULT,  /* default */
    T_ELIF,     /* elif */
    T_ELSE,     /* else */
    T_FALL,     /* fall */
    T_FN,       /* fn */
    T_FOR,      /* for */
    T_IF,       /* if */
    T_IN,       /* in */
    T_IS,       /* is */
    T_NOT,      /* not */
    T_OR,       /* or */
    T_RETURN,   /* return */
    T_STRUCT,   /* struct */
    T_SWITCH,   /* switch */
    T_USE,      /* use */
    T_WHILE,    /* while */

    /* --- LITERALS --- */
    T_BOOL,   /* true */
    T_INT,    /* 42 */
    T_FLOAT,  /* .0 */
    T_STRING, /* "a string" */

    /* --- SYMBOLS --- */
    T_DEQ,       /* == */
    T_NEQ,       /* != */
    T_EQ,        /* = */
    T_PLUSEQ,    /* += */
    T_PLUS,      /* + */
    T_MINUSEQ,   /* -= */
    T_MINUS,     /* - */
    T_STAREQ,    /* *= */
    T_STAR,      /* * */
    T_SLASHEQ,   /* /= */
    T_SLASH,     /* / */
    T_HATEQ,     /* ^= */
    T_HAT,       /* ^ */
    T_AMPEQ,     /* &= */
    T_DAMP,      /* && */
    T_AMP,       /* & */
    T_BAREQ,     /* |= */
    T_DBAR,      /* || */
    T_BAR,       /* | */
    T_LEQ,       /* <= */
    T_LT,        /* < */
    T_GEQ,       /* >= */
    T_GT,        /* > */
    T_DOT,       /* . */
    T_COMMA,     /* , */
    T_COLON,     /* : */
    T_SEMICOLON, /* ; */
    T_ATSYMBOL,  /* @ */
    T_OPAR,      /* ( */
    T_CPAR,      /* ) */
    T_OBRACKET,  /* [ */
    T_CBRACKET,  /* ] */
    T_OBRACES,   /* { */
    T_CBRACES,   /* } */

    T_NTOKENS
} Token;

#ifdef GET_TOKEN_STRINGS
static const char *token_strings[] = {"NULL", "id", "and", "assert", "break",
    "case", "continue", "default", "elif", "else", "fall", "fn", "for", "if",
    "in", "is", "not", "or", "return", "struct", "switch", "use", "while",
    "bool", "int", "float", "string", "deq", "neq", "eq", "pluseq", "plus",
    "minuseq", "minus", "stareq", "star", "slasheq", "slash", "hateq", "hat",
    "ampeq", "damp", "amp", "bareq", "dbar", "bar", "leq", "lt", "geq", "gt",
    "dot", "comma", "colon", "semicolon", "atsymbol", "opar", "cpar",
    "obracket", "cbracket", "obraces", "cbraces", NULL};
#endif

typedef struct {
    size_t ok;
    size_t consumed;
    size_t lineno;
    Token token; /* if ok */
    size_t info; /* if ok */
} Capture;

#endif
