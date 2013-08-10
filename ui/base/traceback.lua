-- $Id: traceback.lua 65945 2008-12-15 06:26:49Z tony $
--__auto_local__start--
local string=string
local debug=debug
--__auto_local__end--

traceback = {}

-- frame count exclude gettop
function traceback.gettop()
	local level = 2 
	while true do
		local info = debug.getinfo(level + 1, "n")
		if info == nil then break end
		level = level + 1
	end

	-- gettop
	level = level - 1
	return level
end

-- frame negative index
function traceback.getindex()
	local curframe = -traceback.gettop() -- curframe
	local retval = curframe + 1 -- caller	
	return retval 
end

function traceback.getinfo( level, what )
	if level < 0 then
		level = ( traceback.gettop() + 1 ) + level
	end

	local info = debug.getinfo( level, what )
	return info
end

function traceback.getsource( level )
	local info = traceback.getinfo( level, "Sl")
	if info == nil then return nil, nil, nil end
	local filename = info.source 
	if string.find(filename, "@") == 1 then
		filename = string.sub(filename, 2)
	else
		filename = filename
	end
	return info.what, filename, info.currentline
end

function traceback.dump( offset )
	local top = traceback.gettop()
	local curframe = traceback.getindex() + 1
	if offset and offset > 0 then
		curframe = curframe + offset
	end
	local indicator = ' '
	for frame = curframe, -1 do
		local level = ( top + 1 ) + frame
		local info = debug.getinfo(level,"nfSlu")
		if info == nil then 
			break
		end

		local i = frame - curframe + 1
		if info.what == "C" then   -- is a C function?
			if info.name ~= nil then
				print(string.format("\t%s%2d[C] : in %s",indicator, i, info.name))
			else
				print(string.format("\t%s%2d[C] :",indicator, i))
			end
			--print(string.format("\t%s%2d[C]",indicator, i))
		else   -- a Lua function
			if info.name ~= nil then
				print(string.format("\t%s%2d %s:%d in %s",indicator, i, info.short_src, info.currentline, info.name))
			else
				print(string.format("\t%s%2d %s:%d",indicator, i, info.short_src, info.currentline))
			end
		end		
	end
end
