LOAD 'syntax_ext.so';

-- ECHO:  Hello World ! 
SYNTAX EXTENSION ($$ Hello World ! $$);

DROP TABLE se_1;

CREATE TABLE se_1 (
  record INT SYNTAX EXTENSION ('index=btree nullable=false')
);

-- COMMENT ON COLUMN se_1.record IS 'index=btree nullable=false'
SYNTAX EXTENSION ('GO');

DROP TABLE se_2;

CREATE TABLE se_2 (
  record INT SYNTAX EXTENSION ('index=btree null=false parser selection')
);

-- ECHO: index=btree null=false parser selection
SYNTAX EXTENSION ('GO');

DROP TABLE se_3;

-- ERROR:  invalid colmeta syntax: expected 2 args, got 0
CREATE TABLE se_3 (
  record INT SYNTAX EXTENSION 'colmeta' ('invalid syntax')
);

-- ERROR:  not GO
SYNTAX EXTENSION 'execute' ('EXECUTE');

-- ECHO: EXECUTE
SYNTAX EXTENSION 'echo' ('EXECUTE');

