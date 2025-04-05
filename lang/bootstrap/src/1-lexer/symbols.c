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
        if (*cur == '=') {
            /* == */
            ++cur;
            OK_TOKEN(T_DEQ);
        }

        /* = */
        OK_TOKEN(T_EQ);
    case '!':
        /* !: != */
        if (*cur++ == '=') {
            /* != */
            OK_TOKEN(T_NEQ);
        }

        break;
    case '+':
        /* +: +, += */
        if (*cur == '=') {
            /* += */
            ++cur;
            OK_TOKEN(T_PLUSEQ);
        }

        /* + */
        OK_TOKEN(T_PLUS);
    case '-':
        /* -: -, -=, -> */
        switch (*cur) {
        case '=':
            /* -= */
            ++cur;
            OK_TOKEN(T_MINUSEQ);
        case '>':
            /* -> */
            ++cur;
            OK_TOKEN(T_ARROW);
        }

        /* - */
        OK_TOKEN(T_MINUS);
    case '*':
        /* *: *, *= */
        if (*cur == '=') {
            /* *= */
            ++cur;
            OK_TOKEN(T_STAREQ);
        }

        /* * */
        OK_TOKEN(T_STAR);
    case '/':
        /* /: /, /= */
        if (*cur == '=') {
            /* /= */
            ++cur;
            OK_TOKEN(T_SLASHEQ);
        }

        /* / */
        OK_TOKEN(T_SLASH);
    case '^':
        /* ^: ^, ^= */
        if (*cur == '=') {
            /* ^= */
            ++cur;
            OK_TOKEN(T_HATEQ);
        }

        /* ^ */
        OK_TOKEN(T_HAT);
    case '&':
        /* &: &, &&, &+, &= */
        switch (*cur) {
        case '&':
            /* && */
            ++cur;
            OK_TOKEN(T_DAMP);
        case '+':
            /* &+ */
            ++cur;
            OK_TOKEN(T_AMPPLUS);
        case '=':
            /* &= */
            ++cur;
            OK_TOKEN(T_AMPEQ);
        default:
            /* & */
            OK_TOKEN(T_AMP);
        }
    case '|':
        /* |: |, ||, |= */
        switch (*cur) {
        case '|':
            /* || */
            ++cur;
            OK_TOKEN(T_DBAR);
        case '=':
            /* |= */
            ++cur;
            OK_TOKEN(T_BAREQ);
        default:
            /* | */
            OK_TOKEN(T_BAR);
        }
    case '<':
        /* <: <, <= */
        if (*cur == '=') {
            /* <= */
            ++cur;
            OK_TOKEN(T_LEQ);
        }

        /* < */
        OK_TOKEN(T_LT);
    case '>':
        if (*cur == '=') {
            /* >= */
            ++cur;
            OK_TOKEN(T_GEQ);
        }

        /* > */
        OK_TOKEN(T_GT);
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
