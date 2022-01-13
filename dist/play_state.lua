function one_argument(str)
  local index = str:find(" ")

  if index == nil then
    return str, ""
  end

  local subcommand = str:sub(0, index - 1)
  str = str:sub(index + 1)

  return subcommand, str
end

function on_enter(p)
  player.send(p, "You are now playing!\n\n\r")
end

function on_exit(p)
  player.send(p, "You are no longer playing.\n\r")
end

function on_input(p, arg)
   local cmd, arguments = one_argument(arg)

   if not command.execute(p, cmd, arguments) then
      player.send(p, "Sorry! [bcyan]" .. arg .. "[reset] isn't a known command.\n\r");
   end
end

function on_tick(p)
end

