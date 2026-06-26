#include "postgres.h"

#include "parser/parser_ext.h"

#include "utils/memutils.h"

typedef struct ParserEntry
{
	const char	*name;
	SyntaxExtensionParser const *parser;
} ParserEntry;

static List *parser_registry = NIL;

#define PARSER_TRY()                               \
	do                                             \
	{                                              \
		MemoryContext ccxt = CurrentMemoryContext; \
		PG_TRY();

#define PARSER_CATCH_SYNTAX_ERROR(handler)               \
	PG_CATCH();                                          \
	{                                                    \
		ErrorData	 *errdata;                           \
		MemoryContext ecxt;                              \
                                                         \
		ecxt	= MemoryContextSwitchTo(ccxt);           \
		errdata = CopyErrorData();                       \
                                                         \
		if (errdata->sqlerrcode == ERRCODE_SYNTAX_ERROR) \
		{                                                \
			handler;                                     \
			FlushErrorState();                           \
		}                                                \
		else                                             \
		{                                                \
			MemoryContextSwitchTo(ecxt);                 \
			PG_RE_THROW();                               \
		}                                                \
	}                                                    \
	PG_END_TRY();                                        \
	}                                                    \
	while (0)

static ParserEntry *
find_parser(const char *name)
{
	ListCell	*lc;
	ParserEntry *e;

	foreach (lc, parser_registry)
	{
		e = lfirst(lc);

		if (strcmp(e->name, name) == 0)
		{
			return e;
		}
	}

	return NULL;
}

void
RegisterSyntaxExtensionParser(const char *name, SyntaxExtensionParser const *parser)
{
	ListCell	 *lc;
	ParserEntry	 *e;
	MemoryContext old_context;

	foreach (lc, parser_registry)
	{
		e = lfirst(lc);

		if (strcmp(e->name, name) == 0)
		{
			ereport(WARNING,
					errmsg("replacing existing parser registration for \"%s\"",
						   name));
			e->parser = parser;
			return;
		}
	}

	old_context = MemoryContextSwitchTo(TopMemoryContext);

	e = palloc(sizeof(ParserEntry));
	e->name = pstrdup(name);
	e->parser = parser;

	parser_registry = lappend(parser_registry, e);

	MemoryContextSwitchTo(old_context);
}

Node *
SE_ParseStatement(const char *name, const char *src)
{
	ListCell	*l;
	ParserEntry *e;
	Node		*n = NULL;

	if (name == NULL)
	{
		foreach (l, parser_registry)
		{
			e = lfirst(l);
			if (e->parser->parse_statement == NULL)
			{
				continue;
			}

			PARSER_TRY();
			{
				n = e->parser->parse_statement(src);
			}
			PARSER_CATCH_SYNTAX_ERROR({ n = NULL; });

			if (n != NULL)
			{
				return n;
			}
		}

		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("none of the available parsers could parse the "
						"input")));
	}
	else
	{
		e = find_parser(name);

		if (e == NULL)
		{
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("unknown parser: %s", name)));
		}

		if (e->parser->parse_statement == NULL)
		{
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("parser \"%s\" does not support statements",
							name)));
		}

		return e->parser->parse_statement(src);
	}
}

void
SE_ProcessColumn(const char *name, const char *src, ColumnExtensionContext *cxt)
{
	ListCell	*l;
	ParserEntry *e;
	bool		 success = false;

	if (name == NULL)
	{
		foreach (l, parser_registry)
		{
			e = lfirst(l);
			if (e->parser->process_column == NULL)
			{
				continue;
			}

			PARSER_TRY();
			{
				success = true;
				e->parser->process_column(src, cxt);
			}
			PARSER_CATCH_SYNTAX_ERROR({ success = false; });

			if (success)
			{
				return;
			}
		}

		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("none of the available parsers could parse the "
						"input")));
	}
	else
	{
		e = find_parser(name);

		if (e == NULL)
		{
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("unknown parser: %s", name)));
		}

		if (e->parser->process_column == NULL)
		{
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("parser \"%s\" does not support column "
							"definitions",
							name)));
		}

		e->parser->process_column(src, cxt);
	}
}
