return function(plr, event)
  local player = lunac.player.get(plr)

  if event.scope == game.event.communicate.scope.ROOM then
    if not game.component.location.has(event.origin) then
      lunac.api.log.error("Entity [" .. event.origin.uuid .. "] does not have location component for room scoped communication")

      return
    end

    local player_entity = player.get_entity(plr)

    if not game.component.name.has(event.origin) then
      lunac.api.log.error("Entity [" .. event.origin.uuid .. "] does not have name component for room scoped communication")
    end

    local origin_name = game.component.name.get(event.origin).name

    if game.component.location.has(player_entity) then
      local origin_room_uuid = game.component.location.get(event.origin).room_uuid
      local player_entity_room_uuid = game.component.location.get(player_entity).room_uuid

      if (origin_room_uuid == player_entity_room_uuid) then
        player.sendln("[bcyan]" .. origin_name .. "[reset] says '" .. event.what .. "'.")
      end
    end
  end
end