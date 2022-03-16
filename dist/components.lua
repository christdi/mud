local components = {}

local register
local add_location
local get_location
local has_location
local add_inventory
local get_inventory
local has_inventory
local add_inventory
local get_description
local has_description

register = function()
  components.location = game.register_component()
  components.inventory = game.register_component()
  components.description = game.register_component()
end

add_location = function(entity, data)
  data.entity = entity.uuid;

  game.add_component(entity, components.location, data)
end

get_location = function(entity)
  return game.get_component(entity, components.location)
end

has_location = function(entity)
  return game.has_component(entity, components.location)
end

add_inventory = function(entity, data)
  data.entity = entity.uuid;

  game.add_component(entity, components.inventory, data)
end

get_inventory = function(entity)
  return game.get_component(entity, components.inventory)
end

has_inventory = function(entity)
  return game.has_component(entity, components.inventory)
end

add_description = function(entity, data)
  data.entity = entity.uuid;

  game.add_component(entity, components.description, data)
end

get_description = function(entity)
  return game.get_component(entity, components.description)
end

has_description = function(entity)
  return game.has_component(entity, components.description)
end

return {
  components = components,
  register = register,
  add_location = add_location,
  get_location = get_location,
  has_location = has_location,
  add_inventory = add_inventory,
  get_inventory = get_inventory,
  has_inventory = has_inventory,
  add_description = add_description,
  get_description = get_description,
  has_description = has_description
} 