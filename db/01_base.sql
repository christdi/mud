CREATE TABLE IF NOT EXISTS script_sandbox_group (
  uuid TEXT PRIMARY KEY,
  filepath TEXT NOT NULL,
  name TEXT NOT NULL,
  description TEXT NOT NULL
);

INSERT INTO script_sandbox_group VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', './dist/sandbox/standard.lua', 'standard', 'Standard script permissions');

CREATE TABLE IF NOT EXISTS script (
  uuid TEXT PRIMARY KEY,
  filepath TEXT NOT NULL
);

INSERT INTO script VALUES('97e0937f-23fe-4f06-9d12-d21b22f79938', './dist/command/repl.lua');
INSERT INTO script VALUES('9f12ba01-d6c7-4e3d-bcff-0a2c92f91764', './dist/command/quit.lua');
INSERT INTO script VALUES('37554136-dc8a-4424-a931-c2c92ae5f38a', './dist/command/shutdown.lua');
INSERT INTO script VALUES('0e7c30fd-e4ef-40e9-8024-1e94814b215e', './dist/command/script.lua');
INSERT INTO script VALUES('ff1b46dd-79aa-4a46-a767-46d281d5c5d0', './dist/command/look.lua');
INSERT INTO script VALUES('d755f666-29e1-458d-8a7d-c6f0439bd3d4', './dist/command/say.lua');
INSERT INTO script VALUES('358776f6-f135-49c0-bb68-9c5f9ef7cd3c', './dist/command/go.lua');
INSERT INTO script VALUES('411c46fb-c819-45e3-94e0-3b22b2ac0edb', './dist/command/entities.lua');
INSERT INTO script VALUES('de20a2df-3d08-4444-a9f2-d01dfe6e0a4d', './dist/command/systems.lua');
INSERT INTO script VALUES('d34c381c-97b9-4df0-8378-9d7ea69455e0', './dist/action/move_room.lua');
INSERT INTO script VALUES('35994580-01fc-4e29-af0c-cabf5c3540b9', './dist/action/speak_room.lua');
INSERT INTO script VALUES('9a162283-addf-4e5e-98b9-0c51d91612d4', './dist/action/teleport_room.lua');


CREATE TABLE IF NOT EXISTS script_group (
  script_uuid TEXT NOT NULL,
  group_uuid TEXT NOT NULL,
  PRIMARY KEY(script_uuid, group_uuid),
  FOREIGN KEY(script_uuid) REFERENCES script(uuid),
  FOREIGN KEY(group_uuid) REFERENCES script_sandbox_group(uuid)
);

INSERT INTO script_group VALUES('97e0937f-23fe-4f06-9d12-d21b22f79938', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('9f12ba01-d6c7-4e3d-bcff-0a2c92f91764', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('37554136-dc8a-4424-a931-c2c92ae5f38a', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('0e7c30fd-e4ef-40e9-8024-1e94814b215e', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('ff1b46dd-79aa-4a46-a767-46d281d5c5d0', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('d755f666-29e1-458d-8a7d-c6f0439bd3d4', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('358776f6-f135-49c0-bb68-9c5f9ef7cd3c', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('411c46fb-c819-45e3-94e0-3b22b2ac0edb', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('de20a2df-3d08-4444-a9f2-d01dfe6e0a4d', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('d34c381c-97b9-4df0-8378-9d7ea69455e0', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('35994580-01fc-4e29-af0c-cabf5c3540b9', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('9a162283-addf-4e5e-98b9-0c51d91612d4', '85438e94-2d7c-437f-aa73-872b844df1cd');


CREATE TABLE IF NOT EXISTS command (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  script_uuid TEXT NOT NULL,
  FOREIGN KEY(script_uuid) REFERENCES script(uuid)
);

INSERT INTO command VALUES('c4dcc9f6-16b3-4ca6-b304-66ab3910a439', 'repl', '97e0937f-23fe-4f06-9d12-d21b22f79938');
INSERT INTO command VALUES('1f8b5793-298f-4794-8557-4b851d668eb8', 'quit', '9f12ba01-d6c7-4e3d-bcff-0a2c92f91764');
INSERT INTO command VALUES('9456c374-81b6-49f7-8295-579001d629c0', 'shutdown', '37554136-dc8a-4424-a931-c2c92ae5f38a');
INSERT INTO command VALUES('fa479582-6465-44bd-8847-2b0971655706', 'script', '0e7c30fd-e4ef-40e9-8024-1e94814b215e');
INSERT INTO command VALUES('9b96a8e3-95d6-4a85-ac0d-a10fe134b112', 'look', 'ff1b46dd-79aa-4a46-a767-46d281d5c5d0');
INSERT INTO command VALUES('5895c644-50f1-4c16-ab61-1ad7c3d694f3', 'l', 'ff1b46dd-79aa-4a46-a767-46d281d5c5d0');
INSERT INTO command VALUES('fd6d43d9-ae97-454c-984a-66939c94617e', 'say', 'd755f666-29e1-458d-8a7d-c6f0439bd3d4');
INSERT INTO command VALUES('c1543b0d-abde-4856-8c71-f5b2792a6f56', 'go', '358776f6-f135-49c0-bb68-9c5f9ef7cd3c');
INSERT INTO command VALUES('32e111d1-3f44-4149-b5a3-0ed821011511', 'entities', '411c46fb-c819-45e3-94e0-3b22b2ac0edb');
INSERT INTO command VALUES('80f56a70-3fce-4e85-8439-77295b5b69a1', 'systems', 'de20a2df-3d08-4444-a9f2-d01dfe6e0a4d');

CREATE TABLE IF NOT EXISTS command_group (
  uuid TEXT PRIMARY KEY,
  description TEXT NOT NULL  
);

INSERT INTO command_group VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', 'Standard');

CREATE TABLE IF NOT EXISTS command_group_command (
  command_group_uuid TEXT NOT NULL,
  command_uuid TEXT NOT NULL,  
  PRIMARY KEY(command_group_uuid, command_uuid),
  FOREIGN KEY(command_group_uuid) REFERENCES command_group(uuid),
  FOREIGN KEY(command_uuid) REFERENCES command(uuid)
);

INSERT INTO command_group_command VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', 'c4dcc9f6-16b3-4ca6-b304-66ab3910a439');
INSERT INTO command_group_command VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', '1f8b5793-298f-4794-8557-4b851d668eb8');
INSERT INTO command_group_command VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', '9456c374-81b6-49f7-8295-579001d629c0');
INSERT INTO command_group_command VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', 'fa479582-6465-44bd-8847-2b0971655706');
INSERT INTO command_group_command VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', '9b96a8e3-95d6-4a85-ac0d-a10fe134b112');
INSERT INTO command_group_command VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', '5895c644-50f1-4c16-ab61-1ad7c3d694f3');
INSERT INTO command_group_command VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', 'fd6d43d9-ae97-454c-984a-66939c94617e');
INSERT INTO command_group_command VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', 'c1543b0d-abde-4856-8c71-f5b2792a6f56');
INSERT INTO command_group_command VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', '32e111d1-3f44-4149-b5a3-0ed821011511');
INSERT INTO command_group_command VALUES('75bd6b07-eea0-44a6-a5a7-26a5beda690f', '80f56a70-3fce-4e85-8439-77295b5b69a1');

CREATE TABLE IF NOT EXISTS entity (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  description TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS user (
  uuid TEXT PRIMARY KEY,
  username TEXT NOT NULL,
  password_hash TEXT NOT NULL
);

CREATE UNIQUE INDEX idx_user_username ON user(username);

INSERT INTO user VALUES('1e4995dc-ddc7-4697-a8d4-76b6aa3939cc', 'christdi', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8');

CREATE TABLE IF NOT EXISTS user_entity (
  user_uuid TEXT NOT NULL,
  entity_uuid TEXT NOT NULL,
  PRIMARY KEY(user_uuid, entity_uuid),
  FOREIGN KEY(user_uuid) REFERENCES user(uuid),
  FOREIGN KEY(entity_uuid) REFERENCES entity(uuid)
);

CREATE TABLE IF NOT EXISTS action (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  script_uuid TEXT NOT NULL,
  FOREIGN KEY(script_uuid) REFERENCES script(uuid)
);

INSERT INTO action VALUES('b7d44e1d-f87f-4d80-89b7-7bb6299877dd', 'move_room', 'd34c381c-97b9-4df0-8378-9d7ea69455e0');
INSERT INTO action VALUES('5a35e3da-d025-422b-91dd-96fee97185ae', "speak_room", '35994580-01fc-4e29-af0c-cabf5c3540b9');
INSERT INTO action VALUES('96f6b005-b65e-4c13-8098-ebdc2227d26e', "teleport_room", '9a162283-addf-4e5e-98b9-0c51d91612d4');