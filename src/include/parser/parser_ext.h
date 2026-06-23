#ifndef PARSER_EXT_H
#define PARSER_EXT_H

#include "nodes/parsenodes.h"

typedef Node *(*user_parser)(const char *str);

extern void RegisterParser(const char *name, user_parser fn);

extern Node *parse_with(const char *str, const char *name);
extern Node *parse_any(const char *str);

#endif							/* PARSER_EXT_H */
