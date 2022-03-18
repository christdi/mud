local initialise
local execute

local _actions

initialise = function(actions)
  _actions = actions
end

execute = function(name, entity, data)
  for _, v in ipairs(_actions) do
    if v.name:lower() == name:lower() then
      return game.do_action(entity, v, data)
    end
  end

  log.error("Could not execute action [ " .. name .. "] as it was not found")
end

return {
  initialise = initialise,
  execute = execute
}