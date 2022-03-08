if (arg == nil or arg == "") then
  player.send(p, "Syntax: script <available>\n\r")

  return
end

local subcommand, arg = one_argument(arg)

if (subcommand:lower() == "available") then
  local scripts = script.available();

  player.send(p, "The following scripts are available to reference.\n\n\r")

  for index, s in ipairs(scripts) do
    player.send(p, "Script: [bcyan]" .. s.path .. "[reset]\n\r")
    player.send(p, "  uuid: [bcyan]" .. s.uuid .. "[reset]\n\r")
    player.send(p, "  groups: [bcyan]" .. join(s.groups) .. "[reset]\n\n\r")
  end

  player.send(p, "\n\r")

  return
end

player.send(p, "Syntax: script <available>\n\r")