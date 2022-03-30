local define

define = function(type, impl)
  if not type then error('Event type must be specified') end

  local impl = impl or {}
  local type = type

  local dispatch

  dispatch = function(args)
    args.type = type

    lunac.api.game.event(args)
  end

  local interface = {
    dispatch = dispatch,
    type = type
  }

  setmetatable(interface, {
    __index = function(self, key)
      if impl[key] then
        return impl[key]
      end

      return rawget(self, key)
    end
  })

  return interface
end

return {
  define = define
}