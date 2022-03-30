local player = lunac.player.get(p)

local print_syntax = function()
  player.sendln("Syntax: entities <delete|list>")
end

if (arg == nil or arg == "") then
  print_syntax()
end

local subcommand, arg = one_argument(arg)

if subcommand:lower() == "list" then
  local entities = lunac.entity.all()

  for _, v in ipairs(entities) do
    local archetypes = {}

    for name, archetype in pairs(game.archetype) do
      if archetype.matches(v) then
        table.insert(archetypes, name)
      end
    end

    player.sendln("[bcyan]uuid[reset]: " .. v.uuid);

    player.sendln("[bgreen]Archetypes[reset]: [bmagenta]" .. join(archetypes, "[reset],[bmagenta] ") .. "[reset]\n")

    for name, component in pairs(game.component) do
      if component.has(v) then
        player.sendln("[bgreen]" .. name .. "[reset]: " .. dump(component.get(v)));
      end
    end

    player.sendln()
  end

  return
end

if subcommand:lower() == "delete" then
  local uuid, arg = one_argument(arg)

  if uuid == nil or uuid == "" then
    player.sendln("Syntax: entities delete <uuid>")

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
    player.sendln("No entity found matching uuid found")
    return
  end 

  game.delete_entity(entity)

  player.sendln("Entity deleted")

  return
end

print_syntax()