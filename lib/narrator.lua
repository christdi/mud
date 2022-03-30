local _narrators = {}

local define

define = function(name)
  if not name then error("narrator name must be specified") end
  
  local _name = name
  local _callbacks = {}
  local _narrator
  local _interface

  local on
  local get_name
  local narrate;
  local deregister
  local get_instance

  on = function(event, callback)
    if not event then error("event name must be specified") end
    if not callback or type(callback) ~= "function" then error("callback must be specified") end

    _callbacks[event.type] = callback
  end

  get_name = function()
    return _name
  end

  narrate = function(plr, event)
    if not event then error("event must be specified") end

    if _callbacks[event.type] then
      _callbacks[event.type](plr, event)
    end
  end

  deregister = function()
    log.info("Deregistering " .. _name .. " narrator")

    game.deregister_narrator(_narrator)

    for i, narrator in ipairs(_narrators) do
      if narrator == _narrator then
        table.remove(_narrators, i)
        break
      end
    end

    _narrator = nil
  end

  get_instance = function()
    return _narrator
  end

  _interface = {
    on = on,
    get_name = get_name,
    narrate = narrate,
    deregister = deregister,
    get_instance = get_instance
  }

  log.info("Registering " .. _name .. " narrator")
  _narrator = game.register_narrator(_interface)
  table.insert(_narrators, _narrator)

  return _interface
end

return {
  define = define
}
