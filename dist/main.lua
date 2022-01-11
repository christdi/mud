function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end

function main()
  game.components = {}

  log.info("Demo MUD initialising")

  game.components.location = game.register_component();
  game.components.has_inventory = game.register_component()
end


function entities_loaded(entities)
   log.info("Entities loaded called: " .. dump(entities))
end


function player_connected(p)
   log.info("Player connected")

   player.send(p, "Hello! Welcome from Lua\n\r")
end


function player_disconnected(p)
   log.info("Player disconnected")

   player.send(p, "Goodbye! Lua will miss you\n\r")
end


function player_input(p, what)
   log.info("Player input: " .. what)

   player.send(p, "Psst, I'm spying on you.  You entered '" .. what .. "'\n\r");
end


function has_inventory_component(entity)
   local ps = db.prepare("SELECT * FROM has_inventory_component WHERE entity_id = ?")
   ps.bind(1, game.get_entity_id(entity))

   if not db.step(ps) then
      return false
   end


end


function load_accounts()
   local ps = db.prepare("SELECT * FROM account")

   while db.step(ps) do
      local field = db.column_text(ps, 0)
      print("Account", field)
   end

   db.finalize(ps)
end