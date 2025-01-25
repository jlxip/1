#include "easy.h"
#include "src/Grammar.h"
#include <stdlib.h>
#include <string.h>

static char *strip(char *str) {
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

static buffer split(char *str, const char *del) {
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

    const char **cur = NULL;
    map all_symbols = NULL; /* map<str, symbol> */
    size_t i;
    size_t ntok;
    Grammar *ret;
    char *freethis, *str;
    buffer lines;
    size_t lineno;
    size_t prio = 1;
    symbol hint = 0;

    map_new_string(&all_symbols, sizeof(symbol), hash_symbol, equal_symbol,
        copy_symbol, destroy_symbol);
    i = 0;
    map_add(all_symbols, "EPSILON", &i);
    ++i;
    cur = tokens;
    while (*cur) {
        map_add(all_symbols, *cur++, &i);
        ++i;
    }
    ntok = i++; /* ntok, later used for augmented start non-terminal symbol */
    cur = nts;
    while (*cur) {
        map_add(all_symbols, *cur++, &i);
        ++i;
    }

    /* Create grammar */
    ret = (Grammar *)malloc(sizeof(Grammar));
    if (!map_has(all_symbols, start))
        throw("invalid start symbol");
    Grammar_new(ret, ntok, i, *map_get(all_symbols, start, symbol));
    Grammar_set_debugging(ret, tokens, nts);

    /* Parse input */
    freethis = str = malloc(strlen(cstr) + 1);
    strcpy(str, cstr);
    lines = split(str, "\n");
    for (lineno = 0; lineno < buffer_num(lines); ++lineno) {
        /* Example: C -> c C 'prod1 | d 'prod2 */
        char *line;
        buffer sides;
        char *strlhs, *rhs;
        const symbol *lhs;
        buffer options;
        size_t i;

        line = *buffer_get(lines, lineno, char *);
        assert(line);
        line = strip(line);
        assert(line);
        if (strlen(line) == 0)
            continue;

        if (*line == '#')
            continue; /* comment */

        if (*line == '%') {
            /* Precedence */
            buffer spaces;
            const char *statement;

            /* Example: %left token token token */
            ++line;
            line = strip(line);
            spaces = split(line, " ");
            assert(buffer_num(spaces) >= 2);
            statement = *buffer_get(spaces, 0, char *);

            if (strcmp(statement, "hint") == 0) {
                /* Hint about next production */
                const char *strsym;
                assert(buffer_num(spaces) == 2);
                strsym = *buffer_get(spaces, 1, const char *);
                hint = *map_get(all_symbols, strsym, symbol);
            } else {
                /* Must be precedence */
                size_t assoc;
                Precedence prec;
                size_t i;

                if (strcmp(statement, "prec") == 0)
                    assoc = UNDEFINED_ASSOC;
                else if (strcmp(statement, "nonassoc") == 0)
                    assoc = NONASSOC;
                else if (strcmp(statement, "left") == 0)
                    assoc = LEFT_ASSOC;
                else if (strcmp(statement, "right") == 0)
                    assoc = RIGHT_ASSOC;
                else
                    throwe("What's this? \"%%%s\"\n", statement);

                prec.assoc = assoc;

                /* Example: token token token */
                for (i = 1; i < buffer_num(spaces); ++i) {
                    const char *strsym;
                    symbol sym;

                    strsym = *buffer_get(spaces, i, const char *);
                    sym = *map_get(all_symbols, strsym, symbol);

                    prec.prio = prio++;
                    map_add(ret->prec, &sym, &prec);
                }
            }

            buffer_out(&spaces);
            continue;
        }

        sides = split(line, "->");
        if (buffer_num(sides) != 2)
            throwe("oops bad grammar, missed -> near line %lu", lineno + 1);
        strlhs = strip(*buffer_get(sides, 0, char *)); /* Example: C */
        rhs = *buffer_get(sides, 1, char *);           /* Example: c C */
        buffer_out(&sides);

        lhs = map_get(all_symbols, strlhs, symbol);
        if (!lhs)
            throwe("inexistent lhs symbol: \"%s\"", strlhs);

        options = split(rhs, "|");
        for (i = 0; i < buffer_num(options); ++i) {
            /* Example: c C 'prod1 */
            char *option;
            buffer namesep;
            char *name;
            buffer symbols;
            buffer syms = NULL;
            size_t j;

            option = *buffer_get(options, i, char *);
            option = strip(option);
            if (strlen(option) == 0)
                throw("oops bad grammar, no option");

            /* Get name for this production (right of quote) */
            namesep = split(option, "'");
            if (buffer_num(namesep) == 2) {
                name = *buffer_get(namesep, 1, char *);
                strip(name);
            } else {
                name = NULL;
            }
            option = *buffer_get(namesep, 0, char *);
            buffer_out(&namesep);
            strip(option);
            /* Example: c C */

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
            if (hint)
                Grammar_add_hint(ret, buffer_num(ret->g) - 1, hint);
            if (name)
                Grammar_add_name(ret, buffer_num(ret->g) - 1, name);
            buffer_out(&symbols);
        }

        buffer_out(&options);
        hint = 0;
    }

    map_out(&all_symbols);
    buffer_out(&lines);
    free(freethis);
    return ret;
}

void grammar_compile(void *ptr) {
    Grammar *g = (Grammar *)ptr;
    Grammar_compile(g);
}

void grammar_out(void *ptr) {
    Grammar *g = (Grammar *)ptr;
    Grammar_out(g);
    free(ptr);
}

/* --- */

void grammar_parse(void *ptr, const TokenData *stream) {
    Grammar *g = (Grammar *)ptr;
    /* Note that TokenData = StreamElement */
    Grammar_parse(g, (StreamElement *)stream);
}
