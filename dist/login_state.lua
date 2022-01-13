substate = {
   GET_USERNAME = 1,
   GET_PASSWORD = 2
}

function on_enter(p)
  player.send(p, "Welcome to [bred]demo[reset] MUD!\n\r")

  local data = player.get_data(p)
  data.login = { substate = substate.GET_USERNAME }

  player.save_data(p, data)
end

function on_exit(p)
  player.send(p, "Login state goodbye!\n\r")
end

function on_input(p, arg)
end

function on_tick(p)
end

