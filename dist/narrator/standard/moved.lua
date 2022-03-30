return function(p, event)
  local player = lunac.player.get(p)
  local player_entity = player.get_entity()

  if game.component.location.has(player_entity) then
    local player_location = game.component.location.get(player_entity);

    if not game.component.description.has(event.portal) then
      lunac.api.log.error("Couldn't narrate as event portal did not have a description")

      return
    end

    local portal_description = game.component.description.get(event.portal)

    if not game.component.name.has(event.entity) then
      lunac.api.log.error("Couldn't narrate as event entity did not have a name")

      return
    end

    local entity_name = game.component.name.get(event.entity).name;

    if game.component.location.has(event.portal) then
      local from_location = game.component.location.get(event.portal)

      if from_location.room_uuid == player_location.room_uuid then
        player.sendln("[bcyan]" .. entity_name .. "[reset] leaves via " .. portal_description.short .. ".")
      end
    end

    if game.component.room_ref.has(event.portal) then
      local to_location = game.component.room_ref.get(event.portal)

      if to_location.ref == player_location.room_uuid then
        player.sendln("[bcyan]" .. entity_name .. "[reset] arrives from " .. portal_description.short .. ".")
      end
    end
  end
end