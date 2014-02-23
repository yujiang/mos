local serpent = require("base.serpent")
local table = require "table"


--local load = require('luabins').load
--local save = require('luabins').save
local save = serpent.short
local load = function(data)
	return true,assert(loadstring("return "..data))()
end

local msg = class("msg")

function msg:prefix_size(is)
	self._prefix_size = is
end

local MAX_PACKAGE = 0xffff
function msg:send(client,...)
	--print("msg.send",save({...}))
	local s = save({...})
	assert(#s <= MAX_PACKAGE)
	if self._prefix_size then
		s = string.format("%04x%s",#s,s)
	end
	client:send(s)
end

local function get_msg(data)
	if #data <= 4 then
		return nil,data
	end
	local num = tonumber(string.sub(data,1,4),16)
	if #data < 4+num then
		return nil,data
	end
	local s = string.sub(data,4+1,num+4)
	data = string.sub(data,4+num+1)
	return s,data
end

function msg:handle(client,data)
	if not self._prefix_size then
		return self:_handle(client,data)
	end

	print("msg:handle",data)
	if client._buffer then
		data = client._buffer..data
		client._buffer = nil
	end
	local s
	while true do
		s,data = get_msg(data)
		if s then
			self:_handle(client,s)
		else
			if data and #data > 0 then
				client._buffer = data
			end
			break
		end
	end
end

function msg:_handle(client,data)
	if not self.handles then
		print("msg:handle not self.handles")
		return
	end
	local ok,args = load(data)
	--print("handle",client._vfd,data,ok,type,args)
	if not ok then
		print("msg:handle not valid data",client._vfd,data)
		return 
	end
	local type = args[1]
	table.remove(args,1)
	--print(type,save(args))
	if not self.handles[type] then
		print("msg:handle not valid data type",client._vfd,data,type)
		return
	end
	--print("handle",client._vfd,data,ok,type,args)
	self.handles[type](client,unpack(args))
	return true
end

function msg:register(type,func)
	if not self.handles then
		self.handles = {}
	end
	self.handles[type] = func
end

return msg