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

  game.log_info("main.lua", "Demo MUD initialising")

  game.components.location = game.register_component();
  game.components.has_inventory = game.register_component()
end


function entities_loaded(entities)
   game.log_info("main.lua", "On entities loaded called: " .. dump(entities))
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