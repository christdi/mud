local player = lunac.player.get(p)

local print_syntax = function()
  player.send("Syntax: entities <delete|list>\n\r")
end

if (arg == nil or arg == "") then
  print_syntax()
end

local subcommand, arg = one_argument(arg)

if subcommand:lower() == "list" then
  local entities = game.get_entities()

  for _, v in ipairs(entities) do

    local archetypes = {}

    if lunac.archetype.goable.matches(v) then
      table.insert(archetypes, "goable")
    end

    if lunac.archetype.observable.matches(v) then
      table.insert(archetypes, "observable")
    end

    player.send("[bcyan]uuid[reset]: " .. v.uuid .. " ([bmagenta]" .. join(archetypes, "[reset],[bmagenta] ") .. "[reset])\n\r");


    if lunac.component.location.has(v) then
      player.send(" [bgreen]location[reset] = " .. dump(lunac.component.location.get(v)) .. "\n\r")
    end

    if lunac.component.name.has(v) then
      player.send(" [bgreen]name[reset] = " .. dump(lunac.component.name.get(v)) .. "\n\r")
    end

    if lunac.component.description.has(v) then
      player.send(" [bgreen]description[reset] = " .. dump(lunac.component.description.get(v)) .. "\n\r")
    end

    if lunac.component.inventory.has(v) then
      player.send(" [bgreen]inventory[reset] = " .. dump(lunac.component.inventory.get(v)) .. "\n\r")
    end

    if lunac.component.room_ref.has(v) then
      player.send(" [bgreen]room ref[reset] = " .. dump(lunac.component.room_ref.get(v)) .. "\n\r")
    end

    if lunac.component.tag.has(v) then
      player.send(" [bgreen]tag[reset] = " .. dump(lunac.component.tag.get(v)) .. "\n\r")
    end

    player.send("\n\r")
  end

  return
end

if subcommand:lower() == "delete" then
  local uuid, arg = one_argument(arg)

  if uuid == nil or uuid == "" then
    player.send("Syntax: entities delete <uuid>\n\r")

    return
  end

  local entities = game.get_entities()
  local entity = nil;

  for _, v in ipairs(entities) do
    if (v.uuid == uuid) then
      entity = v
      break
    end
  end

  if entity == nil then
    player.send("No entity found matching uuid found\n\r")
    return
  end 

  game.delete_entity(entity)

  player.send("Entity deleted\n\r")

  return
end

print_syntax()