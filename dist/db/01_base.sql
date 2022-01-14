CREATE TABLE IF NOT EXISTS script (
  uuid TEXT PRIMARY KEY,
  filepath TEXT NOT NULL,
  allow_std_lib INTEGER NOT NULL,
  allow_db_api INTEGER NOT NULL,
  allow_game_api INTEGER NOT NULL,
  allow_log_api INTEGER NOT NULL,
  allow_player_api INTEGER NOT NULL,
  allow_script_api INTEGER NOT NULL,
  allow_command_api INTEGER NOT NULL
);

INSERT INTO script VALUES('9f12ba01-d6c7-4e3d-bcff-0a2c92f91764', './command.lua', 1, 1, 1, 1, 1, 1, 1);
INSERT INTO script VALUES('96b6080b-5cf6-4781-89ee-e6602f9f274d', './login_state.lua', 1, 1, 1, 1, 1, 1, 1);
INSERT INTO script VALUES('f019f74f-66ce-4e93-8ca5-7f8319f9fa1f', './play_state.lua', 1, 1, 1, 1, 1, 1, 1);

CREATE TABLE IF NOT EXISTS command (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  function TEXT NOT NULL,
  script_uuid TEXT NOT NULL,
  FOREIGN KEY(script_uuid) REFERENCES script(uuid)
);

INSERT INTO command VALUES('1f8b5793-298f-4794-8557-4b851d668eb8', 'quit', 'do_quit', '9f12ba01-d6c7-4e3d-bcff-0a2c92f91764');
INSERT INTO command VALUES('9456c374-81b6-49f7-8295-579001d629c0', 'shutdown', 'do_shutdown', '9f12ba01-d6c7-4e3d-bcff-0a2c92f91764');
INSERT INTO command VALUES('fa479582-6465-44bd-8847-2b0971655706', 'script', 'do_script', '9f12ba01-d6c7-4e3d-bcff-0a2c92f91764');

CREATE TABLE IF NOT EXISTS state (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  on_enter_function TEXT,
  on_exit_function TEXT,
  on_tick_function TEXT,
  on_input_function TEXT,
  script_uuid TEXT NOT NULL,
  FOREIGN KEY(script_uuid) REFERENCES script(uuid)
);

CREATE UNIQUE INDEX idx_state_name ON state(name);

INSERT INTO state VALUES('6c508205-b798-4ede-8353-619d47a0ed94', 'login', 'on_enter', 'on_exit', 'on_tick', 'on_input', '96b6080b-5cf6-4781-89ee-e6602f9f274d');
INSERT INTO state VALUES('88461e14-cdc5-4092-9f96-bc7dc81f2b1d', 'play', 'on_enter', 'on_exit', 'on_tick', 'on_input', 'f019f74f-66ce-4e93-8ca5-7f8319f9fa1f');

CREATE TABLE IF NOT EXISTS entity (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  description TEXT NOT NULL
);

INSERT INTO entity VALUES('32f55645-f7fc-436a-ba9d-afb3c59d3654', 'Lithos', 'Code Monkey');

CREATE TABLE IF NOT EXISTS user (
  uuid TEXT PRIMARY KEY,
  username TEXT NOT NULL,
  password_hash TEXT NOT NULL
);

CREATE UNIQUE INDEX idx_user_username ON user(username);

INSERT INTO user VALUES('1e4995dc-ddc7-4697-a8d4-76b6aa3939cc', 'christdi', 'f21e213ba273c7d686ed7a97dd5ff7bdfd8438ee1edcb0ffcb7529c3a343bb31');

CREATE TABLE IF NOT EXISTS user_entity (
  user_uuid TEXT NOT NULL,
  entity_uuid TEXT NOT NULL,
  PRIMARY KEY(user_uuid, entity_uuid),
  FOREIGN KEY(user_uuid) REFERENCES user(uuid),
  FOREIGN KEY(entity_uuid) REFERENCES entity(uuid)
);

INSERT INTO user_entity VALUES('1e4995dc-ddc7-4697-a8d4-76b6aa3939cc','32f55645-f7fc-436a-ba9d-afb3c59d3654');