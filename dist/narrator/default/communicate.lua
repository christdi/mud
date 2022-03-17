return function(plr, event)
  if event.scope == communicate_event.scope.ROOM then
    if not location_component.has(event.origin) then
      log.error("Entity [" .. event.origin.uuid .. "] does not have location component for room scoped communication")

      return
    end

    local player_entity = player.get_entity(plr)

    if not name_component.has(event.origin) then
      log.error("Entity [" .. event.origin.uuid .. "] does not have name component for room scoped communication")
    end

    local origin_name = name_component.get(event.origin).name

    if location_component.has(player_entity) then
      local origin_room_uuid = location_component.get(event.origin).room_uuid
      local player_entity_room_uuid = location_component.get(player_entity).room_uuid

      if (origin_room_uuid == player_entity_room_uuid) then
        player.send(plr, "\n\r[bcyan]" .. origin_name .. "[reset] says '" .. event.what .. "'.\n\r")
      end
    end
  end
end