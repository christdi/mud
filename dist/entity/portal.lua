local initialise;

initialise = function(self, name, origin, destination, short_description, long_description, tags)
  self.location.room_uuid = origin.uuid
  self.room_ref.ref = destination.uuid
  self.description.short = short_description
  self.description.long = long_description

  for k, v in ipairs(tags) do
    self.tag[k] = v
  end

  return self;
end

return {
  initialise = initialise
}