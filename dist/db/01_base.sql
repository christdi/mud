CREATE TABLE IF NOT EXISTS account (
  username TEXT PRIMARY KEY,
  password_hash TEXT NOT NULL
  );

INSERT INTO account VALUES('christdi','5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8');

CREATE TABLE IF NOT EXISTS script (
  uuid TEXT PRIMARY KEY,
  filepath TEXT NOT NULL,
  allow_std_lib INTEGER NOT NULL,
  allow_db_api INTEGER NOT NULL,
  allow_game_api INTEGER NOT NULL,
  allow_log_api INTEGER NOT NULL,
  allow_player_api INTEGER NOT NULL,
  allow_script_api INTEGER NOT NULL
  );

INSERT INTO script VALUES('9f12ba01-d6c7-4e3d-bcff-0a2c92f91764', './command/quit.lua', 1, 1, 1, 1, 1, 1);
INSERT INTO script VALUES('510a37d6-5e9a-4d77-884b-8a470a8f42a2', './command/shutdown.lua', 1, 1, 1, 1, 1, 1);

CREATE TABLE IF NOT EXISTS command (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  function TEXT NOT NULL,
  script_uuid TEXT NOT NULL,
  FOREIGN KEY(script_uuid) REFERENCES script(uuid)
  );

INSERT INTO command VALUES('1f8b5793-298f-4794-8557-4b851d668eb8', 'quit', 'do_command', '9f12ba01-d6c7-4e3d-bcff-0a2c92f91764');
INSERT INTO command VALUES('9456c374-81b6-49f7-8295-579001d629c0', 'shutdown', 'do_command', '510a37d6-5e9a-4d77-884b-8a470a8f42a2');

CREATE TABLE IF NOT EXISTS entity (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  description TEXT NOT NULL
  );

INSERT INTO entity VALUES('32f55645-f7fc-436a-ba9d-afb3c59d3654', 'Lithos', 'Code Monkey');

CREATE TABLE IF NOT EXISTS account_entity (
  account_username TEXT NOT NULL,
  entity_uuid TEXT NOT NULL,
  PRIMARY KEY(account_username, entity_uuid),
  FOREIGN KEY(account_username) REFERENCES account (username),
  FOREIGN KEY(entity_uuid) REFERENCES entity(uuid)
  );

INSERT INTO account_entity VALUES('christdi','32f55645-f7fc-436a-ba9d-afb3c59d3654');