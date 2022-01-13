substate = {
   GET_ENTITY_NAME = 1,
   GET_ENTITY_DESCRIPTION = 2
}

function on_enter(p)
  player.send(p, "Welcome to [bred]DEMO[reset] MUD!\n\n\r")
  player.send(p, "The login process will allow you to create an entity to control\n\n\r")
  player.send(p, "Please enter a name: ");

  local data = player.get_data(p)
  data.login = { substate = substate.GET_ENTITY_NAME }

  player.save_data(p, data)
end

function on_exit(p)
  player.send(p, "Login state goodbye!\n\r")
end

function on_input(p, arg)
   local data = player.get_data(p);

   if data.login.substate == substate.GET_ENTITY_NAME then
      data.login.entity_name = arg

      player.send(p, "Hello " .. arg .. ", please enter a description: ")

      data.login.substate = substate.GET_ENTITY_DESCRIPTION

      player.save_data(p, data);

      return
   end

   if data.login.substate == substate.GET_ENTITY_DESCRIPTION then
      data.login.entity_description = arg

      local entity = game.new_entity(data.login.entity_name, data.login.entity_description)

      player.send(p, "New entity has been created, assigning you control.\n\r")

      player.change_state(p, "play")
   end


end

function on_tick(p)
end

