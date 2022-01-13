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

function on_enter(p)
  player.send(p, "Login state hello!\n\r")

  log.info("First get data");
  local data = player.get_data(p)
  
  data.test = "hello";
  player.save_data(p, data)

  log.info(dump(data));

  log.info("Second get data");
  data = player.get_data(p)

  log.info("Data test is [" .. data.test .. "]")
end

function on_exit(p)
  player.send(p, "Login state goodbye!\n\r")
end

function on_input(p, arg)
end

function on_tick(p)
  player.send(p, "Tick\n\r")
end
