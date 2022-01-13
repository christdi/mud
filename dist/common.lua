function one_argument(str)
  local index = str:find(" ")

  if index == nil then
    return str, ""
  end

  local subcommand = str:sub(0, index - 1)
  str = str:sub(index + 1)

  return subcommand, str
end

function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end