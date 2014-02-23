local table = require "table"

local socket = class("socket")
local net = require "socket.core"


function socket:send(s)
	assert(self:is_connected(),"self:is_connected()")
	local num,msg,param = self._tcp:send(s)
	if not num and msg ~= "timeout" then
		self._tcp = nil
		self:on_error("send",msg)
		return nil,msg
	end
	return true,num
end

function socket:on_error(where,msg)
	print("on_error",where,msg)
	if self._callback_error then
		self._callback_error(where,msg)
	end
end

function socket:bind_handle(callbackdata,callbackerror)
	self._callback_data = callbackdata
	self._callback_error = callbackerror
end

function socket:connect(ip,port)
	print("socket:connect",ip,port)
	self._ip = ip
	self._port = port
	if not self._timer then
		self._timer = g_timer:add_timer_everyframe(self.ontimer_run,self)
	end
	return self:_connect()
end

function socket:_connect()
	--self._tcp = net.tcp()
	--连接就不必异步了
	local ok,msg = net.connect(self._ip,self._port)	
	if not ok then
		print(msg)
	else
		self._tcp = ok
		self._tcp:settimeout(0)
	end
	return ok,msg
end

function socket:reconnect()
	return self:_connect()
end

function socket:is_connected()
	return self._tcp
end

function socket:close()
	if self._timer then
		self._timer:destroy()
		self._timer = nil
	end
	if not self:is_connected() then
		return
	end
	self._tcp:close()
	--self:on_error("close")
end

function socket:recieve_timeout()
	local __body, __status, __partial = self._tcp:receive("*a")	-- read the package body
	if __body == nil and __status == "timeout" then
		return __partial
	end
	return __body, __status
end


--无法作为消息处理，socket没有把len先发，无法正确的解包
function socket:ontimer_run()
	if not self:is_connected() then
		return true
	end

	while true do
		local __body, __status = self:recieve_timeout()	
		if not __body then
			self:on_error("receive",__status)
			self:close()
		   	return
		end

		if #__body == 0 then
			return true
		end
		--print("body:", __body, "__status:", __status, "__partial:", __partial,self._callback_data)
		if self._callback_data then
			self._callback_data(self,__body)
		end
	end

	return true
end

return socket