local character = player.get_entity(p)
local room = character_entity.get_room(character)
local room_description = description_component.get(room)

local entities = observable_archetype.entities(function(entity)
  local location = location_component.get(entity)

  return location.room_uuid == room.uuid
end)

player.send(p, "[bcyan]" .. room_description.short .. "[reset]\n\n\r")
player.send(p, room_description.long .. "\n\n\r")

for _, entity in ipairs(entities) do
  local description = description_component.get(entity)

  player.send(p, description.long .. " [[bcyan]" .. description.short .. ", " .. entity.uuid .. "[reset]]\n\r")
end

events.dispatch(character_looked_event.new(character))