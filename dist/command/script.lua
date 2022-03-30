local player = lunac.player.get(p)

if (arg == nil or arg == "") then
  player.sendln("Syntax: script <available>")

  return
end

local subcommand, arg = one_argument(arg)

if (subcommand:lower() == "available") then
  local scripts = script.available();

  player.sendln("The following scripts are available to reference.\n")

  for index, s in ipairs(scripts) do
    player.sendln("Script: [bcyan]" .. s.path .. "[reset]")
    player.sendln("  uuid: [bcyan]" .. s.uuid .. "[reset]")
    player.sendln("  groups: [bcyan]" .. join(s.groups) .. "\n")
  end

  player.sendln()

  return
end

player.sendln("Syntax: script <available>")