if (arg == nil or arg == "") then
  player.send(p, "Syntax: go <tag>\n\r")

  return;
end

local entity = player.get_entity(p)
local location = location_component.get(entity)

local entities = goable_archetype.entities(function(goable)
  local lc = location_component.get(goable)
  return lc.room_uuid == location.room_uuid
end)

for _, goable in ipairs(entities) do
  local tags = tag_component.get(goable)

  for _, tag in ipairs(tags) do
    if tag:lower() == arg:lower() then
      local room_ref = room_ref_component.get(goable)

      local current_room = character_entity.get_room(entity)
      local new_room = room_entity.get(room_ref.ref)

      character_entity.set_room(entity, new_room)

      events.dispatch(moved_event.new(entity, goable))
      command.execute(p, "look", "")

      return
    end
  end
end

player.send(p, "There doesn't seem to be a way to go there.\n\r")