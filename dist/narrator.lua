local narrate
local player_connected
local character_looked
local communicate

narrate = function(plr, event)
  local handlers = {
    { event = player_connected_event.TYPE, handler = player_connected },
    { event = character_looked_event.TYPE, handler = character_looked },
    { event = communicate_event.TYPE, handler = communicate }
  }

  for _, v in ipairs(handlers) do
    if v.event == event.type then
      v.handler(plr, event)

      return
    end
  end
end

player_connected = function(plr, event)
end

character_looked = function(plr, event)
  local player_character = player.get_entity(plr)
  local source_character = event.character

  player.send(plr, "\n\r" .. source_character.name  .. " looks around.\n\r")
end

communicate = function(plr, event)
  if event.scope == communicate_event.scope.ROOM then
    if not components_module.has_location(event.origin) then
      log.error("Entity [" .. event.origin.uuid .. "] does not have location component for room scoped communication")

      return
    end

    local player_entity = player.get_entity(plr)

    if components_module.has_location(player_entity) then
      local origin_room_uuid = components_module.get_location(event.origin).room_uuid
      local player_entity_room_uuid = components_module.get_location(player_entity).room_uuid

      if (origin_room_uuid == player_entity_room_uuid) then
        player.send(plr, "\n\r" .. event.origin.name .. " says '" .. event.what .. "'.\n\r")
      end
    end
  end
end

return {
  narrate = narrate
}