#include "lexer.h"

/* Match symbols: =, +, >= */

size_t match_symbol(Capture *ret, const char *cur) {
    const char *begin = cur;
    switch (*cur++) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        /* Numeric literal */
        cur = begin; /* Reset to grab the first number too */
        if (match_numeric(ret, cur))
            return 1;
        break;
    case '"':
        /* String literal */
        /* Do not reset to ommit the first quote */
        if (match_string(ret, cur))
            return 1;
        break;
    case '=':
        /* =: =, == */
        switch (*cur++) {
        case '=':
            /* == */
            OK_TOKEN(T_DEQ);
        default:
            /* = */
            OK_TOKEN(T_EQ);
        }
    case '!':
        /* !: != */
        switch (*cur++) {
        case '=':
            /* != */
            OK_TOKEN(T_NEQ);
        }
    case '+':
        /* +: +, += */
        switch (*cur++) {
        case '=':
            /* += */
            OK_TOKEN(T_PLUSEQ);
        default:
            /* + */
            OK_TOKEN(T_PLUS);
        }
    case '-':
        /* -: -, -= */
        switch (*cur++) {
        case '=':
            /* -= */
            OK_TOKEN(T_MINUSEQ);
        default:
            /* - */
            OK_TOKEN(T_MINUS);
        }
    case '*':
        /* *: *, *= */
        switch (*cur++) {
        case '=':
            /* *= */
            OK_TOKEN(T_STAREQ);
        default:
            /* * */
            OK_TOKEN(T_STAR);
        }
    case '/':
        /* /: /, /=, //, //= */
        switch (*cur++) {
        case '=':
            /* /= */
            OK_TOKEN(T_SLASHEQ);
        case '/':
            /* //: //, //= */
            switch (*cur++) {
            case '=':
                /* //= */
                OK_TOKEN(T_DSLASHEQ);
            default:
                /* // */
                OK_TOKEN(T_DSLASH);
            }
        default:
            /* / */
            OK_TOKEN(T_SLASH);
        }
    case '^':
        /* ^: ^, ^= */
        switch (*cur++) {
        case '=':
            /* ^= */
            OK_TOKEN(T_HATEQ);
        default:
            /* ^ */
            OK_TOKEN(T_HAT);
        }
    case '&':
        /* &: &, &= */
        switch (*cur++) {
        case '=':
            /* &= */
            OK_TOKEN(T_AMPEQ);
        default:
            /* & */
            OK_TOKEN(T_AMP);
        }
    case '<':
        /* <: <, <= */
        switch (*cur++) {
        case '=':
            /* <= */
            OK_TOKEN(T_LEQ);
        default:
            /* < */
            OK_TOKEN(T_LT);
        }
    case '>':
        switch (*cur++) {
        case '=':
            /* >= */
            OK_TOKEN(T_GEQ);
        default:
            /* > */
            OK_TOKEN(T_GT);
        }
    case '.':
        OK_TOKEN(T_DOT);
    case ',':
        OK_TOKEN(T_COMMA);
    case ':':
        OK_TOKEN(T_COLON);
    case ';':
        OK_TOKEN(T_SEMICOLON);
    case '@':
        OK_TOKEN(T_ATSYMBOL);
    case '(':
        OK_TOKEN(T_OPAR);
    case ')':
        OK_TOKEN(T_CPAR);
    case '[':
        OK_TOKEN(T_OBRACKET);
    case ']':
        OK_TOKEN(T_CBRACKET);
    case '{':
        OK_TOKEN(T_OBRACES);
    case '}':
        OK_TOKEN(T_CBRACES);
    }

    return 0;
}
