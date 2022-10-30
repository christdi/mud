local with_origin
local with_destination
local with_short_description
local with_long_description
local with_tags

with_origin = function(self, origin)
  self.location.room_uuid = origin.uuid
  
  return self
end

with_destination = function(self, destination)
  self.room_ref.ref = destination.uuid

  return self
end

with_short_description = function(self, short)
  self.description.short = short
  
  return self
end

with_long_description = function(self, long)
  self.description.long = long
  
  return self
end

with_tags = function(self, tags)
  for k, v in ipairs(tags) do
    self.tag[k] = v
  end

  return self
end

return {
  with_origin = with_origin,
  with_destination = with_destination,
  with_short_description = with_short_description,
  with_long_description = with_long_description,
  with_tags = with_tags
}