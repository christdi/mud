local character = lunac.entity.character.wrap(player.get_entity(p))
local room = character:get_room()

local entities = observable_archetype.entities(function(entity)
  local location = lunac.component.location.get(entity)

  return location.room_uuid == room.uuid
end)

player.send(p, "[bcyan]" .. room.description.short .. "[reset]\n\n\r")
player.send(p, room.description.long .. "\n\n\r")

for _, entity in ipairs(entities) do
  local description = lunac.component.description.get(entity)

  player.send(p, description.long .. " [[bcyan]" .. description.short .. "[reset]]\n\r")
end

lunac.event.character_looked.dispatch({character = character})