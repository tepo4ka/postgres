#include "postgres.h"

#include "parser/parser_ext.h"

typedef struct ParserEntry {
  char const *name;
  user_parser fn;
} ParserEntry;

static List *parser_registry = NIL;

void RegisterParser(char const *name, user_parser fn) {
  ListCell *lc;

  foreach (lc, parser_registry) {
    ParserEntry *e = lfirst(lc);

    if (strcmp(e->name, name) == 0) {
      ereport(WARNING, (errmsg("replacing existing parser registration for \"%s\"", name)));
      e->fn = fn;
      return;
    }
  }

  ParserEntry *e = palloc(sizeof(ParserEntry));

  e->name = pstrdup(name);
  e->fn = fn;

  parser_registry = lappend(parser_registry, e);
}

Node *parse_with(char const *str, char const *name) {
  ListCell *lc;

  foreach (lc, parser_registry) {
    ParserEntry *e = lfirst(lc);
    if (strcmp(e->name, name) == 0)
      return e->fn(str);
  }

  ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("unknown parser: %s", name)));
  return NULL;
}

Node *parse_any(char const *str) {
  ListCell *lc;

  foreach (lc, parser_registry) {
    ParserEntry *e = lfirst(lc);
    Node *result = e->fn(str);
    if (result != NULL)
      return result;
  }

  ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
                  errmsg("none of the available parsers could parse the input")));
  return NULL;
}
