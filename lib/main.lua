lunac = {
   component = require('lib/component'),
   state = require('lib/state'),
   task = require('lib/task'),
   system = require('lib/system'),
   entity = require('lib/entity'),
   event = require('/lib/event')
}

function one_argument(str)
  local index = str:find(" ")

  if index == nil then
    return str, ""
  end

  local subcommand = str:sub(0, index - 1)
  str = str:sub(index + 1)

  return subcommand, str
end

function join(tbl, sep)
   if tbl == nil then
      return ""
   end

   if sep == null then
      sep = " "
   end

   local str = ""

   for k, v in pairs(tbl) do
      local last = tbl[#tbl] == v

      if last then
         str = str .. v
      else
         str = str .. v .. sep
      end
   end

   return str
end

function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. k ..' = ' .. dump(v) .. ', '
      end
      return s .. '} '
   else
      return tostring(o)
   end
end

function filter_array(tbl, filter)
 local j = 1

 for i = 1, #tbl do
   if (filter(tbl[i])) then
     if (i ~= j) then
         tbl[j] = tbl[i];
         tbl[i] = nil;
     end

     j = j + 1;
   else
     tbl[i] = nil;
   end
 end
end