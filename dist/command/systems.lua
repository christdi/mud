local player = lunac.player.get(p)

if (arg == nil or arg == "") then
  player.send("Syntax: systems <list|enable|disable>\n\r")

  return;
end

local subcommand, arg = one_argument(arg)

if subcommand:lower() == "list" then
  for _, v in ipairs(lunac.system.all()) do
    player.send("\n\r[bcyan]" .. v.get_name() .. "\n\r");
    player.send(" [bgreen]uuid[reset] = " .. v.get_uuid() .. "\n\r")
    player.send(" [bgreen]enabled[reset] = " .. tostring(v.is_enabled()) .. "\n\r")
  end

  return
end

if subcommand:lower() == "enable" then
  local uuid, arg = one_argument(arg)

  if uuid == nil or uuid == "" then
    player.send("Syntax: systems enable <uuid>\n\r")

    return
  end

  local system

  for _, v in ipairs(lunac.system.all()) do
    if v.get_uuid() == uuid then
      system = v
    end
  end

  if not system then
    player.send("No system found with that uuid\n\r")

    return
  end

  system.enable()

  player.send("[bcyan]" .. system.get_name() .. "[reset] system enabled")
end

if subcommand:lower() == "disable" then
  local uuid, arg = one_argument(arg)

  if uuid == nil or uuid == "" then
    player.send("Syntax: systems enable <uuid>\n\r")

    return
  end

  local system

  for _, v in ipairs(lunac.system.all()) do
    if v.get_uuid() == uuid then
      system = v
    end
  end

  if not system then
    player.send("No system found with that uuid\n\r")

    return
  end

  system.disable()

  player.send("[bcyan]" .. system.get_name() .. "[reset] system disabled")
end

