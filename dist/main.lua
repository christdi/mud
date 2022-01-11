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

  local entity = game.new_entity("test", "test entity")
  local entity_two = game.new_entity("test 2", "test entity 2")

  game.add_component(entity, game.components.has_inventory, {capacity = 30})
  game.add_component(entity, game.components.location, {room = "1234"})

  game.add_component(entity_two, game.components.has_inventory, {capacity = 20})

  local component_one = game.get_component(entity, game.components.has_inventory)
  print("Entity one components", dump(component_one))

  local location_component = game.get_component(entity, game.components.location)
  print("Entity one location", dump(location_component))

  local component_two = game.get_component(entity_two, game.components.has_inventory)
  print("Entity two components", dump(component_two))

  if game.has_component(entity, game.components.has_inventory) then
   print("Entity one has inventory component")
  end

   if game.has_component(entity, game.components.location) then
      print("Entity one has location component")
   end

   if game.has_component(entity_two, game.components.has_inventory) then
      print("Entity two has inventory component")
   end

   if not game.has_component(entity_two, game.components.location) then
      print("Entity two does not have location component")
   end
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

main()

