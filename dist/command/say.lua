local player = lunac.player.get(p)

if (arg == nil or arg == "") then
  player.sendln("Syntax: say <what>")

  return
end

local character = player.get_entity()

actions.execute("speak_room", character, { origin = character, what = arg })