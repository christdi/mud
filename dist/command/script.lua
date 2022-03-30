local player = lunac.player.get(p)

if (arg == nil or arg == "") then
  player.send("Syntax: script <available>\n\r")

  return
end

local subcommand, arg = one_argument(arg)

if (subcommand:lower() == "available") then
  local scripts = script.available();

  player.send("The following scripts are available to reference.\n\n\r")

  for index, s in ipairs(scripts) do
    player.send("Script: [bcyan]" .. s.path .. "[reset]\n\r")
    player.send("  uuid: [bcyan]" .. s.uuid .. "[reset]\n\r")
    player.send("  groups: [bcyan]" .. join(s.groups) .. "[reset]\n\n\r")
  end

  player.send("\n\r")

  return
end

player.send("Syntax: script <available>\n\r")