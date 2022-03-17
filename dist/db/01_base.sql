CREATE TABLE IF NOT EXISTS script_sandbox_group (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  description TEXT
);

INSERT INTO script_sandbox_group VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'standard', 'Standard script permissions');

CREATE TABLE IF NOT EXISTS script_sandbox_permission (
  uuid TEXT PRIMARY KEY,
  module TEXT,
  method TEXT
);

INSERT INTO script_sandbox_permission VALUES('e290caac-75e2-4928-9ef2-7a9a8aeccad0', NULL, 'ipairs');
INSERT INTO script_sandbox_permission VALUES('3516bc9b-4f44-4783-8090-f1edbb55ce3a', NULL, 'dump');
INSERT INTO script_sandbox_permission VALUES('d8cf45ed-3991-466e-b4d3-618b64c108fa', NULL, 'one_argument');
INSERT INTO script_sandbox_permission VALUES('922e1784-a6a0-4453-b918-4d4768fde2f9', NULL, 'join');
INSERT INTO script_sandbox_permission VALUES('757fbaf1-dd12-40a8-9a79-63c5bab8aa14', 'table',  NULL);
INSERT INTO script_sandbox_permission VALUES('49f313d5-f1fd-472a-8c7b-2fbc26884a0f', 'player', 'send');
INSERT INTO script_sandbox_permission VALUES('98c80490-0735-47a0-a7b9-bf43a85c297b', 'player', 'disconnect');
INSERT INTO script_sandbox_permission VALUES('98c3cc04-4096-4ff4-9b06-5c59efe8fc5c', 'player', 'get_entities');
INSERT INTO script_sandbox_permission VALUES('e6457e13-94d6-4cd5-98d6-69a8301df2a9', 'player', 'set_entity');
INSERT INTO script_sandbox_permission VALUES('bdd147a4-0126-426c-bbb1-f2225a01091e', 'player', 'get_entity');
INSERT INTO script_sandbox_permission VALUES('4d62f24e-d421-45a2-8000-4a24218e5d1a', 'game', 'get_entities');
INSERT INTO script_sandbox_permission VALUES('60d7a4b5-0db0-4fe2-8db1-4d9544384c6a', 'game', 'get_entity');
INSERT INTO script_sandbox_permission VALUES('13014295-405a-4d28-bab9-235f26ae40eb', 'game', 'shutdown');
INSERT INTO script_sandbox_permission VALUES('2aa20983-6781-4700-84bb-9f57462d14e5', 'log', 'trace');
INSERT INTO script_sandbox_permission VALUES('313cb8fb-b357-4046-a32b-ed8660a84c31', 'log', 'debug');
INSERT INTO script_sandbox_permission VALUES('cd503b37-e646-4875-955d-4121ea6678a3', 'log', 'info');
INSERT INTO script_sandbox_permission VALUES('a2986bb9-9646-4bbf-99c9-9daaf13b2f8c', 'log', 'warn');
INSERT INTO script_sandbox_permission VALUES('fee86876-8c75-47fa-b246-60b6050c8517', 'log', 'error');
INSERT INTO script_sandbox_permission VALUES('cf33d257-25a8-4b51-95de-2ad319d898ac', 'script', 'available');
INSERT INTO script_sandbox_permission VALUES('8c3dba6e-53ba-42f0-9ef6-a9aba894d7ef', 'command', 'execute');
INSERT INTO script_sandbox_permission VALUES('51bfe9e5-9c51-43fe-8624-28d307ad0ccc', 'character_entity', NULL);
INSERT INTO script_sandbox_permission VALUES('2a12fce4-cf21-4f08-b381-4c9a13f9184a', 'room_entity', NULL);
INSERT INTO script_sandbox_permission VALUES('5e1f3800-b418-4893-8536-13de35746588', 'events', NULL);
INSERT INTO script_sandbox_permission VALUES('df14283a-83c8-4911-bc39-b40d6b1ac576', 'components', NULL);
INSERT INTO script_sandbox_permission VALUES('e8fcd6a6-89d4-4c79-9095-57c9bb4cd3f5', 'archetypes', NULL);
INSERT INTO script_sandbox_permission VALUES('6b2bc1fc-70eb-4392-9181-fd68fe6b739a', 'character_looked_event', NULL);
INSERT INTO script_sandbox_permission VALUES('2660099c-4bd7-4cda-8f6c-05dce4189ef4', 'communicate_event', NULL);
INSERT INTO script_sandbox_permission VALUES('491c0a33-272b-447c-bf1d-1421a71205ea', 'moved_event', NULL);
INSERT INTO script_sandbox_permission VALUES('fcebfac1-a82c-41f5-b739-cc182865e14e', 'description_component', NULL);
INSERT INTO script_sandbox_permission VALUES('b136fea1-f68c-49da-b8af-1357e8708005', 'inventory_component', NULL);
INSERT INTO script_sandbox_permission VALUES('a7117f0a-e823-4d6d-bec5-da8d1103091b', 'location_component', NULL);
INSERT INTO script_sandbox_permission VALUES('f7814eb4-de10-4ee3-a6aa-7f73d779c0c0', 'tag_component', NULL);
INSERT INTO script_sandbox_permission VALUES('72c27730-27da-4abf-af94-598ce1bf571b', 'room_ref_component', NULL);
INSERT INTO script_sandbox_permission VALUES('69fdb888-7f63-4bbd-b0c1-4f22e4a75ff0', 'name_component', NULL);
INSERT INTO script_sandbox_permission VALUES('c9f39e8b-f82e-44c5-87ca-7f18fa94fa17', 'observable_archetype', NULL);
INSERT INTO script_sandbox_permission VALUES('4da6d6b3-26ac-4b47-bd5b-4beb95dc3147', 'goable_archetype', NULL);

CREATE TABLE IF NOT EXISTS script_sandbox_group_permission (
  group_uuid TEXT NOT NULL,
  permission_uuid TEXT NOT NULL,
  PRIMARY KEY(group_uuid, permission_uuid),
  FOREIGN KEY(group_uuid) REFERENCES script_sandbox_group(uuid),
  FOREIGN KEY(permission_uuid) REFERENCES script_sandbox_permission(uuid)
);

INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'e290caac-75e2-4928-9ef2-7a9a8aeccad0');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '3516bc9b-4f44-4783-8090-f1edbb55ce3a');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'd8cf45ed-3991-466e-b4d3-618b64c108fa');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '922e1784-a6a0-4453-b918-4d4768fde2f9');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '757fbaf1-dd12-40a8-9a79-63c5bab8aa14');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '49f313d5-f1fd-472a-8c7b-2fbc26884a0f');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '98c3cc04-4096-4ff4-9b06-5c59efe8fc5c');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'e6457e13-94d6-4cd5-98d6-69a8301df2a9');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'bdd147a4-0126-426c-bbb1-f2225a01091e');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '4d62f24e-d421-45a2-8000-4a24218e5d1a');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '60d7a4b5-0db0-4fe2-8db1-4d9544384c6a');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '98c80490-0735-47a0-a7b9-bf43a85c297b');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '13014295-405a-4d28-bab9-235f26ae40eb');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '2aa20983-6781-4700-84bb-9f57462d14e5');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '313cb8fb-b357-4046-a32b-ed8660a84c31');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'cd503b37-e646-4875-955d-4121ea6678a3');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'a2986bb9-9646-4bbf-99c9-9daaf13b2f8c');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'fee86876-8c75-47fa-b246-60b6050c8517');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'cf33d257-25a8-4b51-95de-2ad319d898ac');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '8c3dba6e-53ba-42f0-9ef6-a9aba894d7ef');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '51bfe9e5-9c51-43fe-8624-28d307ad0ccc');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '2a12fce4-cf21-4f08-b381-4c9a13f9184a');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '5e1f3800-b418-4893-8536-13de35746588');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'df14283a-83c8-4911-bc39-b40d6b1ac576');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'e8fcd6a6-89d4-4c79-9095-57c9bb4cd3f5');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '6b2bc1fc-70eb-4392-9181-fd68fe6b739a');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '2660099c-4bd7-4cda-8f6c-05dce4189ef4');;
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '491c0a33-272b-447c-bf1d-1421a71205ea');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'fcebfac1-a82c-41f5-b739-cc182865e14e');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'b136fea1-f68c-49da-b8af-1357e8708005');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'a7117f0a-e823-4d6d-bec5-da8d1103091b');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'f7814eb4-de10-4ee3-a6aa-7f73d779c0c0');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '72c27730-27da-4abf-af94-598ce1bf571b');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '69fdb888-7f63-4bbd-b0c1-4f22e4a75ff0');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', 'c9f39e8b-f82e-44c5-87ca-7f18fa94fa17');
INSERT INTO script_sandbox_group_permission VALUES('85438e94-2d7c-437f-aa73-872b844df1cd', '4da6d6b3-26ac-4b47-bd5b-4beb95dc3147');

CREATE TABLE IF NOT EXISTS script (
  uuid TEXT PRIMARY KEY,
  filepath TEXT NOT NULL
);

INSERT INTO script VALUES('9f12ba01-d6c7-4e3d-bcff-0a2c92f91764', './command/quit.lua');
INSERT INTO script VALUES('37554136-dc8a-4424-a931-c2c92ae5f38a', './command/shutdown.lua');
INSERT INTO script VALUES('0e7c30fd-e4ef-40e9-8024-1e94814b215e', './command/script.lua');
INSERT INTO script VALUES('ff1b46dd-79aa-4a46-a767-46d281d5c5d0', './command/look.lua');
INSERT INTO script VALUES('d755f666-29e1-458d-8a7d-c6f0439bd3d4', './command/say.lua');
INSERT INTO script VALUES('358776f6-f135-49c0-bb68-9c5f9ef7cd3c', './command/go.lua');
INSERT INTO script VALUES('411c46fb-c819-45e3-94e0-3b22b2ac0edb', './command/entities.lua');

CREATE TABLE IF NOT EXISTS script_group (
  script_uuid TEXT NOT NULL,
  group_uuid TEXT NOT NULL,
  PRIMARY KEY(script_uuid, group_uuid),
  FOREIGN KEY(script_uuid) REFERENCES script(uuid),
  FOREIGN KEY(group_uuid) REFERENCES script_sandbox_group(uuid)
);

INSERT INTO script_group VALUES('9f12ba01-d6c7-4e3d-bcff-0a2c92f91764', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('37554136-dc8a-4424-a931-c2c92ae5f38a', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('0e7c30fd-e4ef-40e9-8024-1e94814b215e', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('ff1b46dd-79aa-4a46-a767-46d281d5c5d0', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('d755f666-29e1-458d-8a7d-c6f0439bd3d4', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('358776f6-f135-49c0-bb68-9c5f9ef7cd3c', '85438e94-2d7c-437f-aa73-872b844df1cd');
INSERT INTO script_group VALUES('411c46fb-c819-45e3-94e0-3b22b2ac0edb', '85438e94-2d7c-437f-aa73-872b844df1cd');

CREATE TABLE IF NOT EXISTS command (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  script_uuid TEXT NOT NULL,
  FOREIGN KEY(script_uuid) REFERENCES script(uuid)
);

INSERT INTO command VALUES('1f8b5793-298f-4794-8557-4b851d668eb8', 'quit', '9f12ba01-d6c7-4e3d-bcff-0a2c92f91764');
INSERT INTO command VALUES('9456c374-81b6-49f7-8295-579001d629c0', 'shutdown', '37554136-dc8a-4424-a931-c2c92ae5f38a');
INSERT INTO command VALUES('fa479582-6465-44bd-8847-2b0971655706', 'script', '0e7c30fd-e4ef-40e9-8024-1e94814b215e');
INSERT INTO command VALUES('9b96a8e3-95d6-4a85-ac0d-a10fe134b112', 'look', 'ff1b46dd-79aa-4a46-a767-46d281d5c5d0');
INSERT INTO command VALUES('5895c644-50f1-4c16-ab61-1ad7c3d694f3', 'l', 'ff1b46dd-79aa-4a46-a767-46d281d5c5d0');
INSERT INTO command VALUES('fd6d43d9-ae97-454c-984a-66939c94617e', 'say', 'd755f666-29e1-458d-8a7d-c6f0439bd3d4');
INSERT INTO command VALUES('c1543b0d-abde-4856-8c71-f5b2792a6f56', 'go', '358776f6-f135-49c0-bb68-9c5f9ef7cd3c');
INSERT INTO command VALUES('32e111d1-3f44-4149-b5a3-0ed821011511', 'entities', '411c46fb-c819-45e3-94e0-3b22b2ac0edb');

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
