if (arg == nil or arg == "") then
  player.send(p, "Syntax: go <tag>\n\r")

  return;
end

local entity = lunac.entity.character.wrap(player.get_entity(p))
local location = lunac.component.location.get(entity)

local entities = lunac.archetype.goable.entities(function(goable)
  local lc = lunac.component.location.get(goable)
  return lc.room_uuid == location.room_uuid
end)

for _, goable in ipairs(entities) do
  local tags = lunac.component.tag.get(goable)

  for _, tag in ipairs(tags) do
    if tag:lower() == arg:lower() then
      local room_ref = lunac.component.room_ref.get(goable)

      local current_room = entity:get_room()
      local new_room = lunac.entity.room.get(room_ref.ref)

      local success, data = actions.execute("move_room", entity, {from = current_room, to = new_room, portal = goable})

      if success then
        player.send_gmcp(p, "Room", "{ \"uuid\": \"" .. new_room.uuid.. "\" }")

        commands.execute("look", p)
      end

      return
    end
  end
end

player.send(p, "There doesn't seem to be a way to go there.\n\r")