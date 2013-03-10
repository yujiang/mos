
--每一帧都调用，多用于move
local timer_everyframe = class("timer_everyframe")

function timer_everyframe:on_every_frame(pass)
	local t = self.timers
	self.timers = {}
	for _,timer in ipairs(t) do
		if not timer.invalid and timer.func(timer.param,pass) then
			self:add_timer(timer)
		end		
	end
end

function timer_everyframe:create_timer_everyframe()
	self.timers = {}
end

function timer_everyframe:add_timer(t)
	table.insert(self.timers,t)
	--print("timer_everyframe:add_timer",#self.timers)
	--if #self.timers > 10 then
		--print(debug.traceback())
	--end
end

return timer_everyframe

