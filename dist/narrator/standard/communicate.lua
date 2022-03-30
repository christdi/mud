return function(plr, event)
  local player = lunac.player.get(plr)

  if event.scope == lunac.event.communicate.scope.ROOM then
    if not lunac.component.location.has(event.origin) then
      log.error("Entity [" .. event.origin.uuid .. "] does not have location component for room scoped communication")

      return
    end

    local player_entity = player.get_entity(plr)

    if not lunac.component.name.has(event.origin) then
      log.error("Entity [" .. event.origin.uuid .. "] does not have name component for room scoped communication")
    end

    local origin_name = lunac.component.name.get(event.origin).name

    if lunac.component.location.has(player_entity) then
      local origin_room_uuid = lunac.component.location.get(event.origin).room_uuid
      local player_entity_room_uuid = lunac.component.location.get(player_entity).room_uuid

      if (origin_room_uuid == player_entity_room_uuid) then
        player.send("\n\r[bcyan]" .. origin_name .. "[reset] says '" .. event.what .. "'.\n\r")
      end
    end
  end
end