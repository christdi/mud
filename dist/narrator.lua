local narrate
local player_connected
local vision
local sound

narrate = function(plr, event)
  local handlers = {
    { event = event_module.type.PLAYER_CONNECTED, handler = player_connected },
    { event = event_module.type.CHARACTER_LOOKED, handler = character_looked }
  }

  for _, v in ipairs(handlers) do
    if v.event == event.type then
      v.handler(plr, event)

      return
    end
  end
end

player_connected = function(plr, event)
end

character_looked = function(plr, event)
  local player_character = player.get_entity(plr)
  local source_character = event.character

  if source_character.uuid ~= player_character.uuid then
    player.send(plr, "\n\rYou see " .. source_character.name  .. " look around.\n\r")
  end
end

return {
  narrate = narrate
}