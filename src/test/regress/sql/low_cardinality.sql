CREATE TABLE lc1 (
  id int,
  status text LOW CARDINALITY 30
);

SELECT
  attoptions
FROM
  pg_attribute
WHERE
  attrelid = 'lc1'::regclass
  AND attname = 'status';

ALTER TABLE lc1
  ALTER COLUMN status SET LOW CARDINALITY 100;

SELECT
  attoptions
FROM
  pg_attribute
WHERE
  attrelid = 'lc1'::regclass
  AND attname = 'status';

ALTER TABLE lc1
  ALTER COLUMN status RESET LOW CARDINALITY;

SELECT
  attoptions
FROM
  pg_attribute
WHERE
  attrelid = 'lc1'::regclass
  AND attname = 'status';

