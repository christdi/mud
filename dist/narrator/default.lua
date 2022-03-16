local register
local use
local interface

local narrate
local player_connected
local character_looked
local communicate
local moved

local narrator

register = function()
  narrator = game.register_narrator(interface)
end

use = function(p)
  player.set_narrator(p, narrator)
end

narrate = function(plr, event)
  local handlers = {
    { event = player_connected_event.TYPE, handler = player_connected },
    { event = character_looked_event.TYPE, handler = character_looked },
    { event = communicate_event.TYPE, handler = communicate },
    { event = moved_event.TYPE, handler = moved }
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
  local ch = event.character

  player.send(plr, "\n\r" .. ch.name  .. " looks around.\n\r")
end

communicate = function(plr, event)
  if event.scope == communicate_event.scope.ROOM then
    if not location_component.has(event.origin) then
      log.error("Entity [" .. event.origin.uuid .. "] does not have location component for room scoped communication")

      return
    end

    local player_entity = player.get_entity(plr)

    if location_component.has(player_entity) then
      local origin_room_uuid = location_component.get(event.origin).room_uuid
      local player_entity_room_uuid = location_component.get(player_entity).room_uuid

      if (origin_room_uuid == player_entity_room_uuid) then
        player.send(plr, "\n\r" .. event.origin.name .. " says '" .. event.what .. "'.\n\r")
      end
    end
  end
end

moved = function(plr, event)
  local player_entity = player.get_entity(plr)

  if location_component.has(player_entity) then
    local player_location = location_component.get(player_entity);

    if not description_component.has(event.portal) then
      log.error("Couldn't narrate as moved event portal did not have a description")

      return
    end

    local portal_description = description_component.get(event.portal)

    if location_component.has(event.portal) then
      local from_location = location_component.get(event.portal)

      if from_location.room_uuid == player_location.room_uuid then
        player.send(plr, event.entity.name .. " leaves via " .. portal_description.short)
      end
    end

    if room_ref_component.has(event.portal) then
      local to_location = room_ref_component.get(event.portal)

      if to_location.ref == player_location.room_uuid then
        player.send(plr, event.entity.name .. " arrives from " .. portal_description.short)
      end
    end
  end
end

interface = {
  register = register,
  use = use,
  narrate = narrate
}

return interface