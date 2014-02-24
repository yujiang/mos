--[[
   Author: Julio Manuel Fernandez-Diaz
   Date:   January 12, 2007
   (For Lua 5.1)
   
   Modified slightly by RiciLake to avoid the unnecessary table traversal in tablecount()

   Formats tables with cycles recursively to any depth.
   The output is returned as a string.
   References to other tables are shown as values.
   Self references are indicated.

   The string returned is "Lua code", which can be procesed
   (in the case in which indent is composed by spaces or "--").
   Userdata and function keys and values are shown as strings,
   which logically are exactly not equivalent to the original code.

   This routine can serve for pretty formating tables with
   proper indentations, apart from printing them:

      print(table.show(t, "t"))   -- a typical use
   
   Heavily based on "Saving tables with cycles", PIL2, p. 113.

   Arguments:
      t is the table.
      name is the name of the table (optional)
      indent is a first indentation (optional).
--]]

function table.show(t, name, indent, ignores)
   local cart     -- a container
   local autoref  -- for self references

   --[[ counts the number of elements in a table
   local function tablecount(t)
      local n = 0
      for _, _ in pairs(t) do n = n+1 end
      return n
   end
   ]]
   -- (RiciLake) returns true if the table is empty
   local function isemptytable(t) return next(t) == nil end

   local function basicSerialize (o)
      local so = tostring(o)
      if type(o) == "function" then
         local info = debug.getinfo(o, "S")
         -- info.name is nil because o is not a calling level
         if info.what == "C" then
            return string.format("%q", so .. ", C function")
         else 
            -- the information is defined through lines
            return string.format("%q", so .. ", defined in (" ..
                info.linedefined .. "-" .. info.lastlinedefined ..
                ")" .. info.source)
         end
      elseif type(o) == "number" or type(o) == "boolean" then
         return so
      else
         return string.format("%q", so)
      end
   end

   local function addtocart (value, name, indent, saved, field)
      indent = indent or ""
      saved = saved or {}
      field = field or name

      cart = cart .. indent .. field

      if type(value) ~= "table" then
         cart = cart .. " = " .. basicSerialize(value) .. ";\n"
      else
         if saved[value] then
            cart = cart .. " = {}; -- " .. saved[value] 
                        .. " (self reference)\n"
            autoref = autoref ..  name .. " = " .. saved[value] .. ";\n"
         else
            saved[value] = name
            --if tablecount(value) == 0 then
            if isemptytable(value) then
               cart = cart .. " = {};\n"
            else
               cart = cart .. " = {\n"
               for k, v in pairs(value) do
				  if not ignores or not ignores[k] then
					  k = basicSerialize(k)
					  local fname = string.format("%s[%s]", name, k)
					  field = string.format("[%s]", k)
					  -- three spaces between levels
					  addtocart(v, fname, indent .. "   ", saved, field)
				  end
               end
               cart = cart .. indent .. "};\n"
            end
         end
      end
   end

   name = name or "__unnamed__"
   if type(t) ~= "table" then
      return name .. " = " .. basicSerialize(t)
   end
   cart, autoref = "", ""
   addtocart(t, name, indent)
   return cart .. autoref
end

-- Print anything - including nested tables
function table_print (tt, indent, done, ignores)
  done = done or {}
  indent = indent or 0
  local func = io.write
  if type(tt) == "table" then
    for key, value in pairs (tt) do
	  if ignores and ignores[key] or type(key) == "number" and key <= #tt then
	  else
      func(string.rep (" ", indent)) -- indent it
      if type (value) == "table" and not done [value] then
        done [value] = true
        func(string.format("[%s] => table\n", tostring (key)));
        func(string.rep (" ", indent+2)) -- indent it
        func("(\n");
        table_print (value, indent + 4, done, ignores)
        func(string.rep (" ", indent+2)) -- indent it
        func(")\n");
      else
        func(string.format("[%s] => %s\n",
            tostring (key), tostring(value)))
      end
	  end
    end
    for key, value in ipairs (tt) do
	  if ignores and ignores[key] then
	  else
      func(string.rep (" ", indent)) -- indent it
      if type (value) == "table" and not done [value] then
        done [value] = true
        func(string.format("[%s] => table\n", tostring (key)));
        func(string.rep (" ", indent+2)) -- indent it
        func("(\n");
        table_print (value, indent + 4, done, ignores)
        func(string.rep (" ", indent+2)) -- indent it
        func(")\n");
      else
        func(string.format("[%s] => %s\n",
            tostring (key), tostring(value)))
      end
	  end
    end
  else
    func(tt .. "\n")
  end
end

function table_tostring (tt, indent, done, ignores)
  done = done or {}
  indent = indent or 0
  if type(tt) == "table" then
    local sb = {}
    for key, value in pairs (tt) do
	  if ignores and ignores[key] then
	  else
      table.insert(sb, string.rep (" ", indent)) -- indent it
      if type (value) == "table" and not done [value] then
        done [value] = true
        table.insert(sb, "{\n");
        table.insert(sb, table_tostring (value, indent + 2, done, ignores))
        table.insert(sb, string.rep (" ", indent)) -- indent it
        table.insert(sb, "}\n");
      elseif "number" == type(key) then
        table.insert(sb, string.format("\"%s\"\n", tostring(value)))
      else
        table.insert(sb, string.format(
            "%s = \"%s\"\n", tostring (key), tostring(value)))
       end
	  end
    end
    return table.concat(sb)
  else
    return tt .. "\n"
  end
end

function to_string( tbl )
    if  "table" == type( tbl ) then
        return table_tostring(tbl)
    else
        return tostring(tbl)
    end
end

function table_print_kv (tt)
	for k,v in pairs(tt) do
		--if type(v) ~= "function" then
			print(tostring(k).." = "..tostring(v))
		--end
	end
end

function table_kv_size(tt)
	local size = 0
	for _,_ in pairs(tt) do
		size = size + 1
	end
	return size
end

function table_has_value(tt,value)
	for k,v in pairs(tt) do
		if v == value then
			return true
		end
	end
end

function table_remove_i(tt,value)
	for i,v in ipairs(tt) do
		if v == value then
			table.remove(tt,i)
			return true
		end
	end
end