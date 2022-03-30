return function(p, event)
  local player = lunac.player.get(p)
  local ent = event.entity

  if not game.component.name.has(ent) then
    lunac.api.log.error("Teleported entity did not have a name for narration")

    return
  end

  local name = game.component.name.get(ent).name
  local p_ent = player.get_entity(plr)

  if game.component.location.has(p_ent) and game.component.location.has(ent) then
    local p_ent_location = game.component.location.get(p_ent)

    if p_ent.uuid == ent.uuid then
      player.sendln("Your stomach lurches as you are teleported elsewhere.");
      commands.execute("look", p)
    end

    if p_ent_location.room_uuid == event.from.uuid then
      player.sendln("[bcyan]" .. name .. "[reset] disappears in a puff of smoke.")
    end

    if p_ent_location.room_uuid == event.to.uuid then
      player.sendln( "[bcyan]" .. name .. "[reset] appears in a puff of smoke.")
    end
  end
end