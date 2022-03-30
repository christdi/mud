local interface

local on_enter
local on_exit
local on_input
local on_gmcp

local display_characters
local get_username
local get_password
local get_entity_choice
local get_entity_name

-- State hook for when state is entered
--
-- p - instance of player userdata
on_enter = function(p)
   local plr = lunac.player.get(p)

   plr.send("Welcome to [bred]DEMO[reset] MUD!\n\n\r")
   plr.send("Please enter your username: ")

   plr.login = { substate = get_username }
end


-- State hook for when state is exited
--
-- p - instance of player userdata
on_exit = function(p)
   local plr = lunac.player.get(p)

   plr.login = nil;
end


-- State hook for when state receives input
--
-- p - instance of player userdata
-- arg - arguments passed in by player
on_input = function(p, arg)
   local plr = lunac.player.get(p);

   if plr.login.substate == nil then error("player did not have a login substate") end

   plr.login.substate(plr, arg)
end


on_gmcp = function(p, topic, msg)
   local plr = lunac.player.get(p)

   if topic == "Game.Login" then
      if not msg then error("Game.Login topic must have a message") end
      
      plr.login.username = msg.node.username

      get_password(plr, msg.node.password)

      return
   end
end

-- Display player characters to player
--
-- plr - Lua player instance
display_characters = function(plr)
      plr.send("\n\rCharacter Selection\n\r");

      local available_entities = plr.get_available_entities()

      if #available_entities == 0 then
         plr.send("\n\rYou have no characters available to login, please create one to play.\n\r");
         
         return
      end

      for k, v in ipairs(plr.get_available_entities()) do
         if lunac.component.name.has(v) then
            local entity_name = lunac.component.name.get(v)

            plr.send("\t[bcyan]" .. entity_name.name .. "[reset]\n\r")
         end
      end
end


-- Substate method to read player username
--
-- plr - Lua player instance
-- arg - arguments passed in by player
get_username = function(plr, arg)
   plr.login.username = arg

   plr.send("\n\rPlease enter your password: ")
   plr.disable_echo()

   plr.login.substate = get_password;
end


-- Substate method to read player password
--
-- plr - Lua player instance
-- arg - arguments passed in by player
get_password = function(plr, arg)
   if not plr.authenticate(plr.login.username, arg) then
      plr.send("\n\r[bred]Authentication failed[reset]\n\n\rPlease enter your username: ")
      plr.login.substate = get_username
      plr.enable_echo()

      return
   end
   
   plr.send("\n\r[bgreen]Authentication successful![reset]\n\r")
   
   display_characters(plr)
   
   plr.send("\n\rPlease enter the name of the character to play or 'new' to create one: ");
   
   plr.login.substate = get_entity_choice;

   plr.enable_echo()
end


-- Substate method to read character selection
--
-- plr - Lua player instance
-- arg - arguments passed in by player
get_entity_choice = function(plr, arg)
   if arg == "new" then
      plr.send("\n\rPlease enter a name: ")

      plr.login.substate = get_entity_name

      return
   end

   local available_entities = plr.get_available_entities()

   for _, v in ipairs(available_entities) do
      if lunac.component.name.has(v) then
         local entity_name = lunac.component.name.get(v)

         if entity_name.name == arg then
            plr.set_entity(v)
            plr.set_state(lunac.state.play)

            return
         end
      end
   end

   display_characters(plr)

   plr.send("\n\rPlease enter the name of the character to play or 'new' to create one: ");
   
end


-- Substate method to read name of new character
--
-- plr - Lua player instance
-- arg - arguments passed in by player
get_entity_name = function(plr, arg)
   if #lunac.component.name.entities(function(entity)       
      return lunac.component.name.get(entity).name:lower() == arg:lower()
   end) > 0 then
      plr.send(p, "\n\rThat name is already in use.  Please enter another: ")
      
      return
   end

   plr.login.name = arg

   local character = lunac.entity.character.new():initialise(arg, arg, "A generic looking individual")
   character:set_room(game.config.default_room);

   plr.set_entity(character)
   plr.set_state(lunac.state.play)
end


interface = {
   on_enter = on_enter,
   on_exit = on_exit,
   on_input = on_input,
   on_gmcp = on_gmcp
}

return interface