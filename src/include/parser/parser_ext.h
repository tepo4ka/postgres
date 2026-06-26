#ifndef PARSER_EXT_H
#define PARSER_EXT_H

#include "nodes/parsenodes.h"

typedef struct ColumnExtensionContext
{
	const RangeVar *relation;	/* relation which the column is part of */
	ColumnDef  *column;
} ColumnExtensionContext;

typedef Node *(*ParseStatementFunc) (const char *src);
typedef void (*ProcessColumnFunc) (const char *src, ColumnExtensionContext *cxt);

typedef struct SyntaxExtensionParser
{
	ParseStatementFunc parse_statement;
	ProcessColumnFunc process_column;
} SyntaxExtensionParser;

extern void RegisterSyntaxExtensionParser(const char *name, const SyntaxExtensionParser *parser);

extern Node *SE_ParseStatement(const char *name, const char *src);
extern void SE_ProcessColumn(const char *name, const char *src, ColumnExtensionContext *cxt);

#endif							/* PARSER_EXT_H */
