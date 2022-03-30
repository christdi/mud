local player = lunac.player.get(p)

local character = game.entity.character.wrap(player.get_entity())
local room = character:get_room()

local entities = game.archetype.observable.entities(function(entity)
  local location = game.component.location.get(entity)

  return location.room_uuid == room.uuid
end)

player.sendln("[bcyan]" .. room.description.short .. "[reset]\n")
player.sendln(room.description.long .. "\n")

for _, entity in ipairs(entities) do
  local description = game.component.description.get(entity)

  player.sendln(description.long .. " [[bcyan]" .. description.short .. "[reset]]")
end

game.event.character_looked.dispatch({character = character})