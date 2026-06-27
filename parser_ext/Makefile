MODULE = parser_ext
PGFILEDESC = "parsers for SYNTAX EXTENSION blocks"

OBJS = \
	syntax_ext.o \

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)

all: $(GENERATED)

include $(PGXS)
