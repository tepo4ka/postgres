#include "postgres.h"

#include "parser/parser_ext.h"

typedef struct ParserEntry {
  char const *name;
  user_parser fn;
} ParserEntry;

static List *parser_registry = NIL;

void RegisterParser(char const *name, user_parser fn) {
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

  elog(ERROR, "unknown parser: %s", name);
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

  elog(ERROR, "no parser succeded");
  return NULL;
}
