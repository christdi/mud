local player = lunac.player.get(p)

if (arg == nil or arg == "") then
  player.sendln("Syntax: go <tag>")

  return
end

local entity = game.entity.character.wrap(player.get_entity())
local location = game.component.location.get(entity)

local entities = game.archetype.goable.entities(function(goable)
  local lc = game.component.location.get(goable)
  return lc.room_uuid == location.room_uuid
end)

for _, goable in ipairs(entities) do
  local tags = game.component.tag.get(goable)

  for _, tag in ipairs(tags) do
    if tag:lower() == arg:lower() then
      local room_ref = game.component.room_ref.get(goable)

      local current_room = entity:get_room()
      local new_room = game.entity.room.get(room_ref.ref)

      local success, data = actions.execute("move_room", entity, {from = current_room, to = new_room, portal = goable})

      if success then
        player.send_gmcp("Room", "{ \"uuid\": \"" .. new_room.uuid.. "\" }")
        player.execute("look")
      end

      return
    end
  end
end

player.sendln("There doesn't seem to be a way to go there.")