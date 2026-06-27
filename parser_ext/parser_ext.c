#include "postgres.h"

#include <stdarg.h>

#include "fmgr.h"

#include "nodes/parsenodes.h"
#include "parser/parser_ext.h"

PG_MODULE_MAGIC_EXT(.name = "parser_ext",.version = PG_VERSION);

__attribute__((format(printf, 1, 2))) static Node *
select_printf(char const *fmt,...)
{

	A_Const    *aconst;
	ResTarget  *target;
	SelectStmt *stmt;

	va_list		args;
	char		buf[1024];

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	aconst = makeNode(A_Const);
	aconst->val.node.type = T_String;
	aconst->val.sval.sval = pstrdup(buf);
	aconst->location = -1;

	target = makeNode(ResTarget);
	target->name = NULL;
	target->indirection = NIL;
	target->val = (Node *) aconst;
	target->location = -1;

	stmt = makeNode(SelectStmt);
	stmt->targetList = list_make1(target);
	stmt->fromClause = NIL;

	return (Node *) stmt;
}

static Node *to_execute = NULL;

static Node *
execute_parse_statement(char const *src)
{
	Node	   *tmp;

	if (strcmp(src, "GO") == 0)
	{
		if (to_execute == NULL)
		{
			ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("nothing to execute")));
		}
		tmp = to_execute;
		to_execute = NULL;
		return tmp;
	}
	else
	{
		ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR), errmsg("not GO")));
	}
}

static Node *
echo_parse_statement(char const *src)
{
	return select_printf("ECHO: %s", src);
}

static void
echo_process_column(char const *src, ColumnExtensionContext *)
{
	to_execute = select_printf("ECHO: %s", src);
}

static void
colmeta_process_column(char const *src, ColumnExtensionContext *cxt)
{
	char		index[32] = {0};
	char		nullable[8] = {0};
	int			ret;

	ret = sscanf(src, "index=%31s nullable=%7s", index, nullable);
	if (ret != 2)
	{
		ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR),
						errmsg("invalid colmeta syntax: expected 2 args, got %d", ret)));
	}

	to_execute = select_printf("COMMENT ON COLUMN %s.%s IS 'index=%s nullable=%s'",
							   cxt->relation->relname, cxt->column->colname,
							   index[0] ? index : "none", nullable[0] ? nullable : "true");
}

static SyntaxExtensionParser ExecuteParser = {.parse_statement = execute_parse_statement,
.process_column = NULL};

static SyntaxExtensionParser EchoParser = {.parse_statement = echo_parse_statement,
.process_column = echo_process_column};

static SyntaxExtensionParser ColmetaParser = {.parse_statement = NULL,
.process_column = colmeta_process_column};

void
_PG_init(void)
{
	RegisterSyntaxExtensionParser("execute", &ExecuteParser);
	RegisterSyntaxExtensionParser("colmeta", &ColmetaParser);
	RegisterSyntaxExtensionParser("echo", &EchoParser);
}
