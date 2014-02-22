local serpent = require("base.serpent")
local table = require "table"


--local load = require('luabins').load
--local save = require('luabins').save
local save = serpent.short
local load = function(data)
	return true,assert(loadstring("return "..data))()
end

handles = {}
function send(client,...)
	--print("msg.send",save({...}))
	client:send(save({...}))
end

function handle(client,data)
	local ok,args = load(data)
	--print("handle",client._vfd,data,ok,type,args)
	if not ok then
		print("not valid data",client._vfd,data)
		return 
	end
	local type = args[1]
	table.remove(args,1)
	if not handles[type] then
		print("not valid data type",client._vfd,data,type)
		return
	end
	handles[type](client,unpack(args))
end

return {handles = handles, send = send, handle = handle}