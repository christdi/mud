return function(plr, event)
  local ch = event.character

  if not lunac.component.name.has(ch) then
    log.error("Couldn't narrate as entity did not have a name component")

    return
  end

  local ch_name = lunac.component.name.get(ch).name;

  player.send(plr, "\n\r" .. ch_name  .. " looks around.\n\r")
end