function on_enter(p)
  player.send(p, "Login state hello!\n\r")
end

function on_exit(p)
  player.send(p, "Login state goodbye!\n\r")
end

function on_input(p, arg)
end

function on_tick(p, arg)
end
