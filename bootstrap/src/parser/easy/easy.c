#include "src/Grammar.h"
#include <stdlib.h>
#include <string.h>

/* End of line */
bool eol(const char *str) {
    switch (*str) {
    case '\0':
    case '\n':
        return true;
    default:
        return false;
    }
}

char *strip(char *str) {
    char *ret = str;
    size_t len;

    if (!str)
        return NULL;

    len = strlen(str);
    while (len) {
        if (*ret == ' ') {
            ++ret;
            --len;
        } else {
            break;
        }
    }

    str = ret + len - 1;
    while (len) {
        if (*str == ' ') {
            *str = '\0';
            --str;
            --len;
        } else {
            break;
        }
    }

    return ret;
}

buffer split(char *str, const char *del) {
    buffer ret = NULL;
    buffer_new(&ret, sizeof(char *));

    str = strtok(str, del);
    do {
        buffer_push(ret, &str);
    } while ((str = strtok(NULL, del)));

    return ret;
}

void *grammar(const char **tokens, const char **nts, const char *cstr,
    const char *start) {

    map all_symbols = NULL; /* map<str, symbol> */
    size_t i;
    size_t ntok;
    Grammar *ret;
    char *freethis, *str;
    buffer lines;
    size_t lineno;

    map_new_string(&all_symbols, sizeof(symbol), hash_symbol, equal_symbol,
        copy_symbol, destroy_symbol);
    i = 0;
    map_add(all_symbols, "EPSILON", &i);
    ++i;
    while (*tokens) {
        map_add(all_symbols, *tokens++, &i);
        ++i;
    }
    ntok = i++; /* ntok, later used for augmented start non-terminal symbol */
    while (*nts) {
        map_add(all_symbols, *nts++, &i);
        ++i;
    }

    /* Create grammar */
    ret = (Grammar *)malloc(sizeof(Grammar));
    if (!map_has(all_symbols, start))
        throw("invalid start symbol");
    Grammar_new(ret, ntok, i, *map_get(all_symbols, start, symbol));

    /* Parse input */
    freethis = str = malloc(strlen(cstr) + 1);
    strcpy(str, cstr);
    lines = split(str, "\n");
    for (lineno = 0; lineno < buffer_num(lines); ++lineno) {
        /* Example: C -> c C | d */
        char *line;
        buffer sides;
        char *strlhs, *rhs;
        const symbol *lhs;
        buffer options;
        size_t i;

        line = *buffer_get(lines, lineno, char *);
        line = strip(line);
        if (strlen(line) == 0)
            continue;

        sides = split(line, "->");
        if (buffer_num(sides) != 2)
            throw("oops bad grammar, missed ->");
        strlhs = strip(*buffer_get(sides, 0, char *)); /* Example: C */
        rhs = *buffer_get(sides, 1, char *);           /* Example: c C */
        buffer_out(&sides);

        lhs = map_get(all_symbols, strlhs, symbol);
        if (!lhs)
            throwe("inexistent lhs symbol: \"%s\"", strlhs);

        options = split(rhs, "|");
        for (i = 0; i < buffer_num(options); ++i) {
            /* Example: c C */
            char *option;
            buffer symbols;
            buffer syms = NULL;
            size_t j;

            option = *buffer_get(options, i, char *);
            option = strip(option);
            if (strlen(option) == 0)
                throw("oops bad grammar, no option");

            symbols = split(option, " ");

            buffer_new(&syms, sizeof(symbol));
            for (j = 0; j < buffer_num(symbols); ++j) {
                /* Example: c */
                char *strsym;
                const symbol *sym;

                strsym = *buffer_get(symbols, j, char *);
                if (strlen(strsym) == 0)
                    throw("oops bad grammar, no symbol?");

                sym = map_get(all_symbols, strsym, symbol);
                if (!sym)
                    throwe("inexistent rhs symbol: \"%s\"", strsym);

                if (*sym == 0)
                    break; /* Epsilon must be an empty buffer */

                buffer_push(syms, sym);
            }

            Grammar_add(ret, *lhs, syms);
            buffer_out(&symbols);
        }

        buffer_out(&options);
    }

    map_out(&all_symbols);
    buffer_out(&lines);
    free(freethis);
    return ret;
}
