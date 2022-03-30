local _players = {}

local wrap
local new
local remove
local all
local get
local send
local send_all

--
-- Wrap a player light userdata in a table providing convenience methods.
--
wrap = function(plr)
  if not plr then error("player must be specified") end

  local _plr = plr

  local send
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

  --
  -- Send a message to the player
  --
  send = function(what)
    if not what then error("what must be specified") end

    player.send(_plr, what)
  end

  --
  -- Send a message to the player using GMCP
  --
  send_gmcp = function(topic, message)
    if not topic then error("topic must be specified") end

    player.send_gmcp(_plr, topic, message)
  end

  --
  -- Set the player entity
  --
  set_entity = function(entity)
    if not entity then error("entity must be specified") end

    player.set_entity(_plr, entity)
  end

  --
  -- Set the state of the player
  --
  set_state = function(state)
    if not state then error("state must be specified") end

    player.set_state(_plr, state.get_instance())
  end

  --
  -- Set the narrator of the player
  --
  set_narrator = function(narrator)
    if not narrator then error("narrator must be specified") end

    player.set_narrator(_plr, narrator.get_instance())
  end

  --
  -- Get the player entity
  --
  get_entity = function()
    return player.get_entity(_plr)
  end

  --
  -- Get the available entities for the player
  --
  get_available_entities = function()
    return player.get_entities(_plr)
  end

  --
  -- Disable echo for the player
  --
  disable_echo = function()
    player.disable_echo(_plr)
  end

  --
  -- Enable echo for the player
  --
  enable_echo = function()
    player.enable_echo(_plr)
  end

  --
  -- Authenticate the player
  --
  authenticate = function(username, password)
    if not username then error("username must be specified") end
    if not password then error("password must be specified") end

    return player.authenticate(_plr, username, password)
  end

  --
  -- Narrate an event to the player
  --
  narrate = function(event)
    if not event then error("event must be specified") end

    player.narrate(_plr, event)
  end

  --
  -- Disconnect the player
  --
  disconnect = function()
    player.disconnect(_plr)
  end

  return {
    send = send,
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
    disconnect = disconnect
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
send_all = function(msg)
  for _, v in pairs(_players) do
    v.send(msg)
  end
end

return {
  new = new,
  remove = remove,
  all = all,
  get = get,
  send = send,
  send_all = send_all
}