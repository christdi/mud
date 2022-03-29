local define

define = function(name, ...)
  if not name or type(name) ~= "string" then error("archetype name must be specified") end

  local _name = name
  local _arg = { ... }
  local _archetype  

  local entities
  local matches
  
  entities = function(filter)
    local entities = game.get_archetype_entities(_archetype)
  
    if filter ~= nil then
      filter_array(entities, filter)
    end
  
    return entities
  end
  
  matches = function(entity)
    return game.matches_archetype(entity, _archetype)
  end

  local components = {}

  for k, v in ipairs(_arg) do
    _arg[k] = v.component()
  end

  log.info("Registering " .. _name .. " archetype")
  _archetype = game.register_archetype(table.unpack(_arg))
  
  return {
    entities = entities,
    matches = matches
  }  
end

return {
  define = define
}
