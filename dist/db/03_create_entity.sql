CREATE TABLE IF NOT EXISTS entity (
  uuid TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  description TEXT NOT NULL
  );

CREATE TABLE IF NOT EXISTS account_entity (
  account_username TEXT NOT NULL,
  entity_uuid TEXT NOT NULL,
  PRIMARY KEY(account_username, entity_uuid),
  FOREIGN KEY(account_username) references account (username),
  FOREIGN KEY(entity_uuid) references entity(uuid)
);