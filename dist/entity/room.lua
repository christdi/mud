local initialise

initialise = function(self, short, long)
  self.description.short = short
  self.description.long = long
end

return {
  initialise = initialise
}