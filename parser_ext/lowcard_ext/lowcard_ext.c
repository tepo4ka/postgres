#include "postgres.h"

#include "fmgr.h"

#include "my_gramparse.h"

/* Generated from the modified my_gram.y */
#include "my_gram.h"

#include "my_kwlist_d.h"

PG_MODULE_MAGIC_EXT(.name = "lowcard_ext",.version = PG_VERSION);

static int
my_parser(core_yyscan_t scanner)
{
	return my_yyparse(scanner);
}

#define PG_KEYWORD(kwname, value, category, collabel) value,

static const uint16 MyScanKeywordTokens[] = {
#include "my_kwlist.h"
};

#undef PG_KEYWORD

void
_PG_init(void)
{
	Parser_hook = my_parser;
	ScanKeywords_hook = &MyScanKeywords;
	ScanKeywordTokens_hook = MyScanKeywordTokens;
}
