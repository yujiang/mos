
--精确的时间
local timer_map = class("timer_map")

--写简单点算了，因为实际上没有那么多timer？
--如果是服务器的话，要写的更好。
function timer_map:on_every_frame()
	--print("timer_map:on_every_frame",#self.timers)
	local clock = os.clock()
	local t = self.timers
	self.timers = {}
	for _,timer in ipairs(t) do
		local rt = true
		if not timer:is_valid() then
			rt = false
		elseif timer.clock_time < clock then
			rt = timer:call()
			if rt then
				if rt == true then
					rt = timer.time
				end
				timer.clock_time = clock + rt
				--print("new time is",timer.clock_time,rt)
			end
		end

		if rt then
			table.insert(self.timers,timer)
		end
	end
end

function timer_map:create_timer_map()
	self.timers = {}
end

function timer_map:add_timer(time,t)
	--print ("timer_map:add_timer",time,t)
	t.time = time
	table.insert(self.timers,t)
end

return timer_map