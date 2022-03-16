if (arg == nil or arg == "") then
  player.send(p, "Syntax: entities <list>\n\r")

  return;
end

local subcommand, arg = one_argument(arg)

if subcommand:lower() == "list" then
  local entities = game.get_entities()

  for _, v in ipairs(entities) do
    player.send(p, "[bcyan]UUID[reset]: " .. v.uuid .. ", [bcyan]name:[reset] " .. v.name .. ", [bcyan]description[reset]: " .. v.description .. "\n\r");
  end
end