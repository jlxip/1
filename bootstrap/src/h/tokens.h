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
    T_AS,       /* as */
    T_ASSERT,   /* assert */
    T_BREAK,    /* break */
    T_CLASS,    /* class */
    T_CONTINUE, /* continue */
    T_DEF,      /* def */
    T_DEL,      /* del */
    T_ELIF,     /* elif */
    T_ELSE,     /* else */
    T_FOR,      /* for */
    T_FROM,     /* from */
    T_GLOBAL,   /* global */
    T_IF,       /* if */
    T_IMPORT,   /* import */
    T_IN,       /* in */
    T_IS,       /* is */
    T_LAMBDA,   /* lambda */
    T_NOT,      /* not */
    T_OR,       /* or */
    T_RETURN,   /* return */
    T_WHILE,    /* while */
    T_WITH,     /* with */

    /* --- LITERALS --- */
    T_NONE,   /* None */
    T_BOOL,   /* True */
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
    T_DSLASHEQ,  /* //= */
    T_DSLASH,    /* // */
    T_SLASHEQ,   /* /= */
    T_SLASH,     /* / */
    T_HATEQ,     /* ^= */
    T_HAT,       /* ^ */
    T_AMPEQ,     /* &= */
    T_AMP,       /* & */
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

#ifdef CREATE_TOKEN_STRINGS
static const char *token_strings[] = {"NULL", "ID", "AND", "AS", "ASSERT",
    "BREAK", "CLASS", "CONTINUE", "DEF", "DEL", "ELIF", "ELSE", "FOR", "FROM",
    "GLOBAL", "IF", "IMPORT", "IN", "IS", "LAMBDA", "NOT", "OR", "RETURN",
    "WHILE", "WITH", "NONE", "BOOL", "INT", "FLOAT", "STRING", "DEQ", "NEQ",
    "EQ", "PLUSEQ", "PLUS", "MINUSEQ", "MINUS", "STAREQ", "STAR", "DSLASHEQ",
    "DSLASH", "SLASHEQ", "SLASH", "HATEQ", "HAT", "AMPEQ", "AMP", "LEQ", "LT",
    "GEQ", "GT", "DOT", "COMMA", "COLON", "SEMICOLON", "ATSYMBOL", "OPAR",
    "CPAR", "OBRACKET", "CBRACKET", "OBRACES", "CBRACES"};
#endif

typedef struct {
    size_t ok;
    size_t consumed;
    size_t lineno;
    Token token; /* if ok */
    size_t info; /* if ok */
} Capture;

#endif
