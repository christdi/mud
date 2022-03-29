local execute

execute = function()
  local chance = math.random(0, 100);

  if chance == 100 then
    local teleportable_entities = lunac.archetype.teleportable.entities()

    if not teleportable_entities or #teleportable_entities == 0 then
      return
    end

    local random_entity = teleportable_entities[math.random(1, #teleportable_entities)]
    local room_entities = lunac.component.room.entities()
    local random_entity_location = lunac.component.location.get(random_entity)
    local current_room = lunac.entity.room.get(random_entity_location.room_uuid)
    local random_room = room_entities[math.random(1, #room_entities)]

    if not actions.execute("teleport_room", random_entity, { entity = random_entity, from = current_room, to = random_room }) then
      log.error("Failed to teleport entity")
    end
  end
end

return {
  execute = execute
}
