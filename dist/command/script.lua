function one_argument(str)
  local index = str:find(" ")

  if index == nil then
    return str, ""
  end

  local subcommand = str:sub(0, index - 1)
  str = str:sub(index + 1)

  return subcommand, str
end

function do_command(p, arg)
  if (arg == nil or arg == "") then
    player.send(p, "Syntax: script <load|unload|loaded|available>\n\r")

    return
  end

  local subcommand, arg = one_argument(arg)

  if (subcommand:lower() == "load") then
    local uuid, arg = one_argument(arg)

    if uuid == nil or uuid == "" then
      player.send(p, "Syntax: script load <uuid>\n\r")

      return
    end

    script.load(uuid)

    player.send(p, "Script with uuid " .. uuid .. " loaded.\n\r")

    return
  end

  if (subcommand:lower() == "unload") then
    local uuid, arg = one_argument(arg)

    if uuid == nil or uuid == "" then
      player.send(p, "Syntax: script unload <uuid>\n\r")

      return
    end

    script.unload(uuid)

    player.send(p, "Script with uuid " .. uuid .. " unloaded.\n\r")

    return
  end

  if (subcommand:lower() == "loaded") then
    local scripts = script.loaded();

    player.send(p, "Scripts loaded\n\n\r")

    for index, s in ipairs(scripts) do
      player.send(p, "uuid: [bcyan]" .. s.uuid .. "[reset] path: [bcyan]" .. s.path .. "[reset]\n\r")
    end

    player.send(p, "\n\r")

    return
  end

  if (subcommand:lower() == "available") then
    local scripts = script.available();

    player.send(p, "Scripts available\n\n\r")

    for index, s in ipairs(scripts) do
      player.send(p, "uuid: [bcyan]" .. s.uuid .. "[reset] path: [bcyan]" .. s.path .. "[reset]\n\r")
    end

    player.send(p, "\n\r")

    return
  end
end