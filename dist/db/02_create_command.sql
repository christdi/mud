CREATE TABLE IF NOT EXISTS command (
  name TEXT PRIMARY KEY,
  function TEXT NOT NULL
  );

INSERT INTO command VALUES('entity', 'function_entity');
INSERT INTO command VALUES('shutdown', 'function_shutdown');
INSERT INTO command VALUES('i', 'function_inventory');
INSERT INTO command VALUES('inventory', 'function_inventory');
INSERT INTO command VALUES('l', 'function_look');
INSERT INTO command VALUES('look', 'function_look');
INSERT INTO command VALUES('quit', 'function_quit');
INSERT INTO command VALUES('say', 'function_say');