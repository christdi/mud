local _players = {}

local wrap
local new
local remove
local all
local get
local send
local send_all
local sendln_all

--
-- Wrap a player light userdata in a table providing convenience methods.
--
wrap = function(plr)
  if not plr then error("player must be specified") end

  local _plr = plr

  local send
  local sendln
  local send_gmcp
  local set_entity
  local set_state
  local set_narrator
  local get_entity
  local get_available_entities
  local disable_echo
  local enable_echo
  local authenticate
  local narrate
  local disconnect
  local add_cmd_group
  local rem_cmd_group
  local execute

  --
  -- Send a message to the player
  --
  send = function(what)
    if not what then error("what must be specified") end

    lunac.api.player.send(_plr, what)
  end

  --
  -- Send a message to the player and append a newline
  --
  sendln = function(what)
    local what = what or ""

    lunac.api.player.send(_plr, what .. "\n\r")
  end

  --
  -- Send a message to the player using GMCP
  --
  send_gmcp = function(topic, message)
    if not topic then error("topic must be specified") end

    lunac.api.player.send_gmcp(_plr, topic, message)
  end

  --
  -- Set the player entity
  --
  set_entity = function(entity)
    if not entity then error("entity must be specified") end

    lunac.api.player.set_entity(_plr, entity)
  end

  --
  -- Set the state of the player
  --
  set_state = function(state)
    if not state then error("state must be specified") end

    lunac.api.player.set_state(_plr, state.get_instance())
  end

  --
  -- Set the narrator of the player
  --
  set_narrator = function(narrator)
    if not narrator then error("narrator must be specified") end

    lunac.api.player.set_narrator(_plr, narrator.get_instance())
  end

  --
  -- Get the player entity
  --
  get_entity = function()
    return lunac.api.player.get_entity(_plr)
  end

  --
  -- Get the available entities for the player
  --
  get_available_entities = function()
    return lunac.api.player.get_entities(_plr)
  end

  --
  -- Disable echo for the player
  --
  disable_echo = function()
    lunac.api.player.disable_echo(_plr)
  end

  --
  -- Enable echo for the player
  --
  enable_echo = function()
    lunac.api.player.enable_echo(_plr)
  end

  --
  -- Authenticate the player
  --
  authenticate = function(username, password)
    if not username then error("username must be specified") end
    if not password then error("password must be specified") end

    return lunac.api.player.authenticate(_plr, username, password)
  end

  --
  -- Narrate an event to the player
  --
  narrate = function(event)
    if not event then error("event must be specified") end

    lunac.api.player.narrate(_plr, event)
  end

  --
  -- Disconnect the player
  --
  disconnect = function()
    lunac.api.player.disconnect(_plr)
  end

  add_cmd_group = function(group)
    if not group then error("group must be specified") end

    lunac.api.player.add_command_group(_plr, group)
  end

  rem_cmd_group = function(group)
    if not group then error("group must be specified") end

    lunac.api.player.remove_command_group(_plr, group)
  end

  execute = function(name, arguments)
    if not name then error("name must be specified") end

    local index = nil

    if (string.sub(name, -2, -2) == ':') then
      index = tonumber(string.sub(name, -1, -1))

      if (index == nil) then return { invalid = true } end

      name = string.sub(name, 1, -3)
    end

    local cmds = lunac.api.player.get_commands(_plr, name)

    if (#cmds == 0) then return { none = true } end
    if (index == nil and #cmds > 1) then return { multiple = true, count = #cmds } end
    if (index ~= nil and index > #cmds) then return { invalid = true } end    
    if (#cmds == 1) then index = 1 end

    lunac.api.player.execute_command(_plr, cmds[index], arguments or "")

    return { success = true }
  end

  return {
    send = send,
    sendln = sendln,
    send_gmcp = send_gmcp,
    set_entity = set_entity,
    set_state = set_state,
    set_narrator = set_narrator,
    get_entity = get_entity,
    get_available_entities = get_available_entities,
    disable_echo = disable_echo,
    enable_echo = enable_echo,
    authenticate = authenticate,
    narrate = narrate,
    disconnect = disconnect,
    add_cmd_group = add_cmd_group,
    rem_cmd_group = rem_cmd_group,
    execute = execute
  }
end

--
-- Adds a new player to the list of players and returns the wrapped player.
--
new = function(plr)
  if not plr then error("player must be specified") end

  local _plr = wrap(plr)
  _players[plr.uuid] = _plr

  return _plr
end

--
-- Removes a player from the list of players.
--
remove = function(plr)
  if not plr then error("player must be specified") end

  if _players[plr.uuid] then
    _players[plr.uuid] = nil
  end
end

--
-- Returns all players contained in the list of players.
--
all = function()
  return _players
end

--
-- Searches for a player by uuid and if found, returns the wrapped player.
--
get = function(plr)
  if not plr then error("plr must be specified") end

  if _players[plr.uuid] then
    return _players[plr.uuid]
  end
end

--
-- Sends a message to all player.
--
send_all = function(msg)
  for _, v in pairs(_players) do
    v.send(msg)
  end
end

--
-- Sends a message to all player and appends a newline.
--
sendln_all = function(msg)
  local msg = msg or ""

  for _, v in pairs(_players) do
    v.sendln(msg)
  end
end

return {
  new = new,
  remove = remove,
  all = all,
  get = get,
  send = send,
  send_all = send_all,
  sendln_all = sendln_all
}