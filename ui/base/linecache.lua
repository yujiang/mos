--$Id: linecache.lua 65945 2008-12-15 06:26:49Z tony $
--__auto_local__start--
local string=string
local table=table
local io=io
local tonumber=tonumber
--__auto_local__end--

linecache = 
{
	files = {}
}

function linecache.clear()
	linecache.files = {}
end

function linecache.getline( filename, lineno )
	local lines = linecache.getlines(filename)	
	if lines == nil then return end
	lineno = tonumber(lineno)
	if lineno >= 1 and lineno <= #lines then		
		return lines[lineno]
	end
end

function linecache.getlines( filename )
	if linecache.files[filename] then
		return linecache.files[filename].lines
	else
		return linecache.updatecache(filename)
	end		
end

function linecache.updatecache(filename)
	if not filename then return {} end
	if linecache.files[filename] then
		linecache.files[filename] = nil
	end

	local lines = {}	

	if os.path.exists( filename ) then
		local fin = io.open(filename, 'r')
		for line in fin:lines() do
			local _,_,line = string.find(line,"([^\n]*)")
			table.insert( lines , line )
		end
		fin:close()
	end

	linecache.files[filename] = {}
	linecache.files[filename].lines = lines
	return lines
end


