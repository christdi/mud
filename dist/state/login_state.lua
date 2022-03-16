local register
local use
local interface

local on_enter
local on_exit
local on_input
local populate_characters
local display_characters
local get_username
local get_password
local get_entity_choice
local get_entity_name

local state

-- Register this state
register = function()
   state = game.register_state(interface)
end

-- Switches a player to this state
--
-- p - instance of player userdata
use = function(p)
   player.set_state(p, state)
end

-- State hook for when state is entered
--
-- p - instance of player userdata
on_enter = function(p)
   player.send(p, "Welcome to [bred]DEMO[reset] MUD!\n\n\r")
   player.send(p, "Please enter your username: ")

   local data = game.players[p.uuid];

   data.login = { substate = get_username }
end


-- State hook for when state is exited
--
-- p - instance of player userdata
on_exit = function(p)
   local data = game.players[p.uuid];

   data.login = nil;
end


-- State hook for when state receives input
--
-- p - instance of player userdata
-- arg - arguments passed in by player
on_input = function(p, arg)
   local data = game.players[p.uuid];

   if data.login.substate == nil then
      log.error("Player did not have a substate")

      return;
   end

   data.login.substate(p, arg, data)
end


-- Populate player data with entities available to them
--
-- p - instance of player userdata
-- data - reference to player data
populate_characters = function(p, data)
   local entities = player.get_entities(p)

   if #entities > 0 then
      data.entity = {};

      for _, entity in ipairs(entities) do
         data.entity[entity.name] = entity;
      end
   end
end


-- Display player characters to player
--
-- p - instance of player userdata
-- data - reference to player data
display_characters = function(p, data)
   if data.entity ~= nil then
      player.send(p, "\n\rCharacter Selection\n\r");

      for k, v in pairs(data.entity) do
         player.send(p, "\n\r[bcyan]" .. v.name .. "[reset] - " .. v.description .. "\n\r");
      end
   end
end


-- Substate method to read player username
--
-- p - instance of player userdata
-- arg - arguments passed in by player
-- data - reference to player data
get_username = function(p, arg, data)
   data.login.username = arg

   player.send(p, "\n\rPlease enter your password: ")

   data.login.substate = get_password;
end


-- Substate method to read player password
--
-- p - instance of player userdata
-- arg - arguments passed in by player
-- data - reference to player data
get_password = function(p, arg, data)
   if player.authenticate(p, data.login.username, arg) then
      player.send(p, "\n\r[bgreen]Authentication successful![reset]\n\r")

      populate_characters(p, data)
      display_characters(p, data)

      player.send(p, "\n\rPlease enter the name of the character to play or 'new' to create one: ");

      data.login.substate = get_entity_choice;
   else
      player.send(p, "\n\r[bred]Authentication failed[reset]\n\n\rPlease enter your username: ")

      data.login.substate = get_username
   end
end


-- Substate method to read character selection
--
-- p - instance of player userdata
-- arg - arguments passed in by player
-- data - reference to player data
get_entity_choice = function(p, arg, data)
   if arg == "new" then
      player.send(p, "\n\rPlease enter a name: ")

      data.login.substate = get_entity_name
   else
      if data.entity ~= nil then
         for _, entity in pairs(data.entity) do
            if entity.name:lower() == arg:lower() then
               player.set_entity(p, entity);

               play_state.use(p)

               return
            end
         end
      end

      display_characters(p, data)

      player.send(p, "\n\rPlease enter the name of the character to play or 'new' to create one: ");
   end
end


-- Substate method to read name of new character
--
-- p - instance of player userdata
-- arg - arguments passed in by player
-- data - reference to player data
get_entity_name = function(p, arg, data)
   data.login.name = arg

   local character = character_entity.new(arg, arg, "A generic looking individual")
   character_entity.set_room(character, game.config.default_room);

   player.set_entity(p, character)

   play_state.use(p);
end


interface = {
   register = register,
   use = use,
   on_enter = on_enter,
   on_exit = on_exit,
   on_input = on_input
}

return interface