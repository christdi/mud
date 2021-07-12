#include "mud/ecs/entity_id.h"

entity_id_t entity_id() {
  entity_id_t id;

  generate_uuid(id.uuid, UUID_SIZE);

  return id;
}
