local player = lunac.player.get(p)

if (arg == nil or arg == "") then
  player.sendln("Syntax: systems <list|enable|disable>")

  return;
end

local subcommand, arg = one_argument(arg)

if subcommand:lower() == "list" then
  for _, v in ipairs(lunac.system.all()) do
    player.sendln("\n\r[bcyan]" .. v.get_name());
    player.sendln(" [bgreen]uuid[reset] = " .. v.get_uuid())
    player.sendln(" [bgreen]enabled[reset] = " .. tostring(v.is_enabled()))
  end

  return
end

if subcommand:lower() == "enable" then
  local uuid, arg = one_argument(arg)

  if uuid == nil or uuid == "" then
    player.sendln("Syntax: systems enable <uuid>")

    return
  end

  local system

  for _, v in ipairs(lunac.system.all()) do
    if v.get_uuid() == uuid then
      system = v
    end
  end

  if not system then
    player.sendln("No system found with that uuid")

    return
  end

  system.enable()

  player.sendln("[bcyan]" .. system.get_name() .. "[reset] system enabled")
end

if subcommand:lower() == "disable" then
  local uuid, arg = one_argument(arg)

  if uuid == nil or uuid == "" then
    player.sendln("Syntax: systems enable <uuid>")

    return
  end

  local system

  for _, v in ipairs(lunac.system.all()) do
    if v.get_uuid() == uuid then
      system = v
    end
  end

  if not system then
    player.sendln("No system found with that uuid")

    return
  end

  system.disable()

  player.sendln("[bcyan]" .. system.get_name() .. "[reset] system disabled")
end

