if (arg == nil or arg == "") then
  player.send(p, "Syntax: entities <list>\n\r")

  return;
end

local subcommand, arg = one_argument(arg)

if subcommand:lower() == "list" then
  local entities = game.get_entities()

  for _, v in ipairs(entities) do
    player.send(p, "[bcyan]uuid[reset]: " .. v.uuid .. "\n\r");

    if goable_archetype.matches(v) then
      player.send(p, " [bmagenta]goable[reset]\n\r")
    end

    if observable_archetype.matches(v) then
      player.send(p, " [bmagenta]observable[reset]\n\r")
    end

    if location_component.has(v) then
      player.send(p, " [bgreen]location[reset] = " .. dump(location_component.get(v)) .. "\n\r")
    end

    if name_component.has(v) then
      player.send(p, " [bgreen]name[reset] = " .. dump(name_component.get(v)) .. "\n\r")
    end

    if description_component.has(v) then
      player.send(p, " [bgreen]description[reset] = " .. dump(description_component.get(v)) .. "\n\r")
    end

    if inventory_component.has(v) then
      player.send(p, " [bgreen]inventory[reset] = " .. dump(inventory_component.get(v)) .. "\n\r")
    end

    if room_ref_component.has(v) then
      player.send(p, " [bgreen]room ref[reset] = " .. dump(room_ref_component.get(v)) .. "\n\r")
    end

    if tag_component.has(v) then
      player.send(p, " [bgreen]tag[reset] = " .. dump(tag_component.get(v)) .. "\n\r")
    end

    player.send(p, "\n\r")
  end
end