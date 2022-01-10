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
  mud.components = {}

  mud.log_info("main.lua", "Demo MUD initialising")

  mud.components.location = mud.register_component();
  mud.components.has_inventory = mud.register_component()

  local entity = mud.new_entity("test", "test entity")
  local entity_two = mud.new_entity("test 2", "test entity 2")

  mud.add_component(entity, mud.components.has_inventory, {capacity = 30})
  mud.add_component(entity, mud.components.location, {room = "1234"})

  mud.add_component(entity_two, mud.components.has_inventory, {capacity = 20})

  local component_one = mud.get_component(entity, mud.components.has_inventory)
  print("Entity one components", dump(component_one))

  local location_component = mud.get_component(entity, mud.components.location)
  print("Entity one location", dump(location_component))

  local component_two = mud.get_component(entity_two, mud.components.has_inventory)
  print("Entity two components", dump(component_two))
end


main()

