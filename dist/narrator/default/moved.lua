return function(plr, event)
  local player_entity = player.get_entity(plr)

  if lunac.component.location.has(player_entity) then
    local player_location = lunac.component.location.get(player_entity);

    if not lunac.component.description.has(event.portal) then
      log.error("Couldn't narrate as event portal did not have a description")

      return
    end

    local portal_description = lunac.component.description.get(event.portal)

    if not lunac.component.name.has(event.entity) then
      log.error("Couldn't narrate as event entity did not have a name")

      return
    end

    local entity_name = lunac.component.name.get(event.entity).name;

    if lunac.component.location.has(event.portal) then
      local from_location = lunac.component.location.get(event.portal)

      if from_location.room_uuid == player_location.room_uuid then
        player.send(plr, "\n\r[bcyan]" .. entity_name .. "[reset] leaves via " .. portal_description.short .. ".\n\r")
      end
    end

    if lunac.component.room_ref.has(event.portal) then
      local to_location = lunac.component.room_ref.get(event.portal)

      if to_location.ref == player_location.room_uuid then
        player.send(plr, "\n\r[bcyan]" .. entity_name .. "[reset] arrives from " .. portal_description.short .. ".\n\r")
      end
    end
  end
end