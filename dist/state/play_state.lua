local function on_enter(p)
  player.send(p, "You are now playing!\n\n\r")
end

local function on_exit(p)
  player.send(p, "You are no longer playing.\n\r")
end

local function on_input(p, arg)
   local cmd, arguments = one_argument(arg)

   if not command.execute(p, cmd, arguments) then
      player.send(p, "Sorry! [bcyan]" .. arg .. "[reset] isn't a known command.\n\r");
   end
end

local function on_output(p, output)
  local entity = player.get_entity(p)

  player.send(p, "\n\r" .. entity.name .. " > ")
end

local function on_event(p, event)
  player.narrate(p, event);
end

return {
  on_enter = on_enter,
  on_exit = on_exit,
  on_input = on_input,
  on_output = on_output,
  on_event = on_event
}