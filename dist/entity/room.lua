local new;
local get;

new = function(name, short_description, long_description)
  local room = game.new_entity();

  room_component.add(room, {})

  inventory_component.add(room, {})

  description_component.add(room, {
    short = short_description,
    long = long_description
  })

  return room
end

get = function(uuid)
  return game.get_entity(uuid)
end

return {
  new = new,
  get = get
}