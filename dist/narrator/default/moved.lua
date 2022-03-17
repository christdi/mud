return function(plr, event)
  local player_entity = player.get_entity(plr)

  if location_component.has(player_entity) then
    local player_location = location_component.get(player_entity);

    if not description_component.has(event.portal) then
      log.error("Couldn't narrate as event portal did not have a description")

      return
    end

    local portal_description = description_component.get(event.portal)

    if not name_component.has(event.entity) then
      log.error("Couldn't narrate as event entity did not have a name")

      return
    end

    local entity_name = name_component.get(event.entity).name;

    if location_component.has(event.portal) then
      local from_location = location_component.get(event.portal)

      if from_location.room_uuid == player_location.room_uuid then
        player.send(plr, "\n\r[bcyan]" .. entity_name .. "[reset] leaves via " .. portal_description.short .. ".\n\r")
      end
    end

    if room_ref_component.has(event.portal) then
      local to_location = room_ref_component.get(event.portal)

      if to_location.ref == player_location.room_uuid then
        player.send(plr, "\n\r[bcyan]" .. entity_name .. "[reset] arrives from " .. portal_description.short .. ".\n\r")
      end
    end
  end
end