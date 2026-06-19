-- Example of the LOW CARDINALITY syntax extension.
-- Note that only syntax is correct here, so expect other errors.
-- Consult the LOAD documentation for other path options.
LOAD '/absolute/path/to/lowcard_ext.so';

CREATE TABLE lowcard (
  path TEXT,
  name TEXT LOW CARDINALITY 30
);

ALTER TABLE lowcard
  ALTER COLUMN path SET LOW CARDINALITY 240;

ALTER TABLE lowcard
  ALTER COLUMN name RESET LOW CARDINALITY;

SELECT
  c.relname AS table_name,
  a.attname AS column_name,
  a.attnum,
  a.atttypid::regtype,
  a.attoptions
FROM
  pg_attribute a
  JOIN pg_class c ON c.oid = a.attrelid
WHERE
  c.relname = 'lowcard'
  AND a.attnum > 0
  AND NOT a.attisdropped;

