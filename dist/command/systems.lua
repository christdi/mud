if (arg == nil or arg == "") then
  player.send(p, "Syntax: systems <list|enable|disable>\n\r")

  return;
end

local subcommand, arg = one_argument(arg)

if subcommand:lower() == "list" then
  for _, v in ipairs(systems.systems()) do
    local info = v.info()
    player.send(p, "\n\r[bcyan]" .. info.name .. "\n\r");
    player.send(p, " [bgreen]uuid[reset] = " .. info.uuid .. "\n\r")
    player.send(p, " [bgreen]enabled[reset] = " .. tostring(info.enabled) .. "\n\r")
  end

  return
end

if subcommand:lower() == "enable" then
  local uuid, arg = one_argument(arg)

  if uuid == nil or uuid == "" then
    player.send(p, "Syntax: systems enable <uuid>\n\r")

    return
  end

  local system

  for _, v in ipairs(systems.systems()) do
    local info = v.info()

    if info.uuid == uuid then
      system = v
    end
  end

  if not system then
    player.send(p, "No system found with that uuid\n\r")

    return
  end

  local info = system.info()

  system.enable()

  player.send(p, "[bcyan]" .. info.name .. "[reset] system enabled")
end

if subcommand:lower() == "disable" then
  local uuid, arg = one_argument(arg)

  if uuid == nil or uuid == "" then
    player.send(p, "Syntax: systems enable <uuid>\n\r")

    return
  end

  local system

  for _, v in ipairs(systems.systems()) do
    local info = v.info()

    if info.uuid == uuid then
      system = v
    end
  end

  if not system then
    player.send(p, "No system found with that uuid\n\r")

    return
  end

  local info = system.info()

  system.disable()

  player.send(p, "[bcyan]" .. info.name .. "[reset] system disabled")
end

