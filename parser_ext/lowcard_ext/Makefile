MODULE_big = lowcard_ext
PGFILEDESC = "lowcard_ext - add LOW CARDINALITY syntax support"

OBJS = \
	lowcard_ext.o \
	my_gram.o \

GENERATED = my_gram.c my_gram.h my_kwlist_d.h

include .env

PG_CONFIG = ${PGSQL}/bin/pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)

all: $(GENERATED)

my_kwlist_d.h:
	$(PERL) tools/gen_keywordlist.pl --varname MyScanKeywords my_kwlist.h

my_gram.c my_gram.h: my_gram.y
	$(BISON) -d -o my_gram.c my_gram.y

my_gram.o: my_gram.c my_gram.h

lowcard_ext.o: my_gram.h my_kwlist_d.h

EXTRA_CLEAN += $(GENERATED)

include $(PGXS)
