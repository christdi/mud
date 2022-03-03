local substate = {
   GET_USERNAME = 1,
   GET_PASSWORD = 2,
   GET_ENTITY_NAME = 3,
   GET_ENTITY_DESCRIPTION = 4
}

local function get_username(p, arg, data)
   data.login.username = arg

   player.send(p, "\n\rPlease enter your password: ")

   data.login.substate = substate.GET_PASSWORD
end

local function get_password(p, arg, data)
   if player.authenticate(p, data.login.username, arg) then
      player.send(p, "\n\rAuthentication success\n\n\rPlease enter an entity name: ")

      data.login.substate = substate.GET_ENTITY_NAME
   else
      player.send(p, "\n\rAuthentication failed\n\n\rPlease enter your username: ")

      data.login.substate = substate.GET_USERNAME
   end
end

local function get_entity_name(p, arg, data)
   data.login.entity_name = arg

   player.send(p, "Please enter an entity description: ")

   data.login.substate = substate.GET_ENTITY_DESCRIPTION
end

local function get_entity_description(p, arg, data)
   data.login.entity_description = arg

   game.new_entity(data.login.entity_name, data.login.entity_description)

   player.set_state(p, game.states.play)
end

local function on_enter(p)
   player.send(p, "Welcome to [bred]DEMO[reset] MUD!\n\n\r")
   player.send(p, "Please enter your username: ")

   local data = game.players[player.uuid(p)];
   data.login = { substate = substate.GET_USERNAME };
end

local function on_exit(p)
end

local function on_input(p, arg)
   local data = game.players[player.uuid(p)];

   if data.login.substate == substate.GET_USERNAME then
      return get_username(p, arg, data)
   end

   if data.login.substate == substate.GET_PASSWORD then
      return get_password(p, arg, data)
   end

   if data.login.substate == substate.GET_ENTITY_NAME then
      return get_entity_name(p, arg, data)
   end

   if data.login.substate == substate.GET_ENTITY_DESCRIPTION then
      return get_entity_description(p, arg, data)
   end
end

return {
   on_enter = on_enter,
   on_exit = on_exit,
   on_input = on_input
}