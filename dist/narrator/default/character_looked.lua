return function(plr, event)
  local ch = event.character

  if not name_component.has(ch) then
    log.error("Couldn't narrate as entity did not have a name component")

    return
  end

  local ch_name = name_component.get(ch).name;

  player.send(plr, "\n\r" .. ch_name  .. " looks around.\n\r")
end