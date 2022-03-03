local function narrate(plr, event)
  if event.type == event_module.type.PLAYER_CONNECTED then
    player.send(plr, "Someone has connected!\n\r")
  else
    player.send(plr, "Somewhere, something is happening.\n\r");
  end
end

return {
  narrate = narrate
}