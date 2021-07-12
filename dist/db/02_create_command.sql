CREATE TABLE IF NOT EXISTS command (
  name TEXT PRIMARY KEY,
  function TEXT NOT NULL
  );

INSERT INTO command VALUES('say', 'function_say');