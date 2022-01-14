substate = {
   GET_USERNAME = 1,
   GET_PASSWORD = 2,
   GET_ENTITY_NAME = 3,
   GET_ENTITY_DESCRIPTION = 4
}

function with_player_data(func, p, arg, data)
   func(p, arg, data)

   player.save_data(p, data)
end

function on_enter(p)
  player.send(p, "Welcome to [bred]DEMO[reset] MUD!\n\n\r")
  player.send(p, "Please enter your username: ")

  local data = player.get_data(p)
  data.login = { substate = substate.GET_USERNAME }

  player.save_data(p, data)
end

function on_exit(p)
end

function on_input(p, arg)
   local data = player.get_data(p)

   if data.login.substate == substate.GET_USERNAME then
      return with_player_data(get_username, p, arg, data)
   end

   if data.login.substate == substate.GET_PASSWORD then
      return with_player_data(get_password, p, arg, data)
   end

   if data.login.substate == substate.GET_ENTITY_NAME then
      return with_player_data(get_entity_name, p, arg, data)
   end

   if data.login.substate == substate.GET_ENTITY_DESCRIPTION then
      return with_player_data(get_entity_description, p, arg, data)
   end
end

function on_tick(p)
end

function get_username(p, arg, data)
   data.login.username = arg

   player.send(p, "\n\rPlease enter your password: ")

   data.login.substate = substate.GET_PASSWORD   
end

function get_password(p, arg, data)
   if player.authenticate(p, data.login.username, arg) then
      player.send(p, "\n\rAuthentication success\n\n\rPlease enter an entity name: ")

      data.login.substate = substate.GET_ENTITY_NAME
   else
      player.send(p, "\n\rAuthentication failed\n\n\rPlease enter your username: ")

      data.login.substate = substate.GET_USERNAME
   end
end

function get_entity_name(p, arg, data)
   data.login.entity_name = arg

   player.send(p, "Please enter an entity description: ")

   data.login.substate = substate.GET_ENTITY_DESCRIPTION
end

function get_entity_description(p, arg, data)
   data.login.entity_description = arg

   game.new_entity(data.login.entity_name, data.login.entity_description)

   player.change_state(p, "play")   
end