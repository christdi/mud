local player = lunac.player.get(p)

local character = lunac.entity.character.wrap(player.get_entity())
local room = character:get_room()

local entities = lunac.archetype.observable.entities(function(entity)
  local location = lunac.component.location.get(entity)

  return location.room_uuid == room.uuid
end)

player.send("[bcyan]" .. room.description.short .. "[reset]\n\n\r")
player.send(room.description.long .. "\n\n\r")

for _, entity in ipairs(entities) do
  local description = lunac.component.description.get(entity)

  player.send(description.long .. " [[bcyan]" .. description.short .. "[reset]]\n\r")
end

lunac.event.character_looked.dispatch({character = character})