local new;

new = function(name, short_description, long_description)
  local room = game.new_entity(name, "room");

  components_module.add_inventory(room, {})

  components_module.add_description(room, {
    short = short_description,
    long = long_description
  })

  return room
end

return {
  new = new
}