local new;

new = function(name, short_description, long_description)
  local room = game.new_entity(name, "room");

  inventory_component.add(room, {})

  description_component.add(room, {
    short = short_description,
    long = long_description
  })

  return room
end

return {
  new = new
}