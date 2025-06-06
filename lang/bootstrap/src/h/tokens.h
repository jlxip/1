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
    T_ARROW,    /* -> */
    T_ASSERT,   /* assert */
    T_BREAK,    /* break */
    T_KBYTE,    /* byte */
    T_CASE,     /* case */
    T_CONTINUE, /* continue */
    T_DEFAULT,  /* default */
    T_ELIF,     /* elif */
    T_ELSE,     /* else */
    T_EXTERN,   /* extern */
    T_FALL,     /* fall */
    T_FN,       /* fn */
    T_FOR,      /* for */
    T_IF,       /* if */
    T_IMPL,     /* impl */
    T_IN,       /* in */
    T_KBOOL,    /* bool */
    T_KFLOAT,   /* float */
    T_KPTR,     /* ptr */
    T_KSTRING,  /* string */
    T_KWORD,    /* word */
    T_LET,      /* let */
    T_NOT,      /* not */
    T_OR,       /* or */
    T_RETURN,   /* return */
    T_SIZEOF,   /* sizeof */
    T_STRUCT,   /* struct */
    T_SWITCH,   /* switch */
    T_USE,      /* use */
    T_WHILE,    /* while */

    /* --- LITERALS --- */
    T_BOOL,   /* true */
    T_WORD,   /* 42 */
    T_FLOAT,  /* .0 */
    T_STRING, /* "a string" */

    /* --- SYMBOLS --- */
    T_DEQ,       /* == */
    T_NEQ,       /* != */
    T_EQ,        /* = */
    T_PLUSEQ,    /* += */
    T_DPLUS,     /* ++ */
    T_PLUS,      /* + */
    T_MINUSEQ,   /* -= */
    T_DMINUS,    /* -- */
    T_MINUS,     /* - */
    T_DSTAREQ,   /* **= */
    T_DSTAR,     /* ** */
    T_STAREQ,    /* *= */
    T_STAR,      /* * */
    T_SLASHEQ,   /* /= */
    T_SLASH,     /* / */
    T_PERCEQ,    /* %= */
    T_PERC,      /* % */
    T_HATEQ,     /* ^= */
    T_HAT,       /* ^ */
    T_AMPEQ,     /* &= */
    T_DAMP,      /* && */
    T_AMPPLUS,   /* &+ */
    T_AMP,       /* & */
    T_BAREQ,     /* |= */
    T_BAR,       /* | */
    T_LEQ,       /* <= */
    T_LT,        /* < */
    T_GEQ,       /* >= */
    T_GT,        /* > */
    T_TILDE,     /* ~ */
    T_QUESTION,  /* ? */
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
} TokenId;

#ifdef GET_TOKEN_STRINGS
static const char *token_strings[] = {"NULL", "id", "and", "arrow", "assert",
    "break", "kbyte", "case", "continue", "default", "elif", "else", "extern",
    "fall", "fn", "for", "if", "impl", "in", "kbool", "kfloat", "kptr",
    "kstring", "kword", "let", "not", "or", "return", "sizeof", "struct",
    "switch", "use", "while", "bool", "word", "float", "string", "deq", "neq",
    "eq", "pluseq", "dplus", "plus", "minuseq", "dminus", "minus", "dstareq",
    "dstar", "stareq", "star", "slasheq", "slash", "perceq", "perc", "hateq",
    "hat", "ampeq", "damp", "ampplus", "amp", "bareq", "bar", "leq", "lt",
    "geq", "gt", "tilde", "question", "dot", "comma", "colon", "semicolon",
    "atsymbol", "opar", "cpar", "obracket", "cbracket", "obraces", "cbraces",
    NULL};
#endif

typedef struct {
    size_t lineno;
    TokenId id;
    union {
        size_t word;
        char *str;
        double d;
    } data;
} Token;

typedef buffer Tokens; /* buffer<Token> */
typedef size_t iToken; /* position in Tokens */

#endif
