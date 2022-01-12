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

  script.load("9f12ba01-d6c7-4e3d-bcff-0a2c92f91764")
  script.load("510a37d6-5e9a-4d77-884b-8a470a8f42a2")
  print(dump(script.loaded()))
end

function entities_loaded(entities)
end


function player_connected(p)
end


function player_disconnected(p)
end


function player_input(p, what)
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