return function(p, event)
  local player = lunac.player.get(p)
  local ent = event.entity

  if not lunac.component.name.has(ent) then
    log.error("Teleported entity did not have a name for narration")

    return
  end

  local name = lunac.component.name.get(ent).name
  local p_ent = player.get_entity(plr)

  if lunac.component.location.has(p_ent) and lunac.component.location.has(ent) then
    local p_ent_location = lunac.component.location.get(p_ent)

    if p_ent.uuid == ent.uuid then
      player.send("\n\rYour stomach lurches as you are teleported elsewhere.\n\r");
      commands.execute("look", p)
    end

    if p_ent_location.room_uuid == event.from.uuid then
      player.send("\n\r[bcyan]" .. name .. "[reset] disappears in a puff of smoke.\n\r")
    end

    if p_ent_location.room_uuid == event.to.uuid then
      player.send( "\n\r[bcyan]" .. name .. "[reset] appears in a puff of smoke.\n\r")
    end
  end
end