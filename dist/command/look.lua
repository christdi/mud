local character = player.get_entity(p)
local room = character_entity.get_room(character)
local room_description = description_component.get(room)

local entities = archetypes.get_entities_can_describe_in_room()
local descriptions = {}

for _, v in ipairs(entities) do
  local location = location_component.get(v)
  
  if location.room_uuid == room.uuid then
    local description = description_component.get(v)
    table.insert(descriptions, description)
  end
end

player.send(p, "[bcyan]" .. room_description.short .. "[reset]\n\n\r")
player.send(p, room_description.long .. "\n\n\r")

for _, v in ipairs(descriptions) do
  if (v.entity ~= character.uuid) then
    player.send(p, v.long .. " [[bcyan]" .. v.short .. "[reset]]\n\r")
  end
end

events.dispatch(character_looked_event.new(character))