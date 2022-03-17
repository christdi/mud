local initialise
local execute

local actions

initialise = function(acts)
  actions = acts
end

execute = function(name, entity, data)
  for _, v in ipairs(actions) do
    if v.name:lower() == name:lower() then
      return game.do_action(entity, v, data)
    end
  end
end

return {
  initialise = initialise,
  execute = execute
}