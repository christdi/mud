if (arg == nil or arg == "") then
  player.send(p, "Syntax: say <what>\n\r")

  return
end

local character = player.get_entity(p)

event_module.dispatch(communicate_event.new(character, arg, communicate_event.scope.ROOM))