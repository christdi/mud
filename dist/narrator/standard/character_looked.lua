return function(plr, event)
  local player = lunac.player.get(plr)
  local ch = event.character

  if not game.component.name.has(ch) then
    lunac.api.log.error("Couldn't narrate as entity did not have a name component")

    return
  end

  local ch_name = game.component.name.get(ch).name;

  player.sendln("[bcyan]" .. ch_name  .. "[reset] looks around.")
end