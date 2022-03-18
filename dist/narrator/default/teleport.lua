return function(plr, event)
  local ent = event.entity

  if not name_component.has(ent) then
    log.error("Teleported entity did not have a name for narration")

    return
  end

  local name = name_component.get(ent).name
  local p_ent = player.get_entity(plr)

  if location_component.has(p_ent) and location_component.has(ent) then
    local p_ent_location = location_component.get(p_ent)

    if p_ent.uuid == ent.uuid then
      player.send(plr, "\n\rYour stomach lurches as you are teleported elsewhere.\n\r");
      commands.execute("look", plr)
    end

    if p_ent_location.room_uuid == event.from.uuid then
      player.send(plr, "\n\r[bcyan]" .. name .. "[reset] disappears in a puff of smoke.\n\r")
    end

    if p_ent_location.room_uuid == event.to.uuid then
      player.send(plr, "\n\r[bcyan]" .. name .. "[reset] appears in a puff of smoke.\n\r")
    end
  end
end