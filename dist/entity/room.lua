local with_short_description
local with_long_description

with_short_description = function(self, short)
  self.description.short = short
  
  return self
end

with_long_description = function(self, long)
  self.description.long = long
  
  return self
end

return {
  with_short_description = with_short_description,
  with_long_description = with_long_description
}