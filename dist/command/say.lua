if (arg == nil or arg == "") then
  player.send(p, "Syntax: say <what>\n\r")

  return
end

local character = player.get_entity(p)

actions.execute("speak_room", character, { origin = character, what = arg })