
--切片的时间，比如1秒运行1000个npc的ai
--那么这个切片是100，就10个npc的ai
local timer_vector = class("timer_vector")

function on_every_frame_vector(time,v,clock)
	if v.num == 0 then
		return
	end
	--如果v.num个数小的话，是极其不精确的。
	local loop_num = math.ceil(clock / time * v.num)
	if loop_num > v.num then
		loop_num = v.num --should max_loop_num
	end

	local i = v.next
	local maxi 

	while(true) do
		if v[i] then
			timer = v[i]
			if not timer.invalid and timer.func(timer.param) then
				-- just pass
			else
				v[i] = nil
				v.num = v.num - 1
			end
			loop_num = loop_num - 1
			if loop_num == 0 then
				v.next = i+1
				return
			end
			maxi = i
		end

		i = i + 1
		if i > v.max then
			i = 1
			if maxi then
				v.max = maxi
			end
		end
	end
end

function timer_vector:on_every_frame(clock)
	for time,v in pairs(self.timers) do
		on_every_frame_vector(time,v,clock)
	end	
end

function timer_vector:create_timer_vector()
	self.timers = {}
end

function timer_vector:add_timer(time,t)
	if not self.timers[time] then
		self.timers[time] = {num = 0,next = 1,max = 0}
	end
	local tb = self.timers[time]
	for i=1,tb.num+1 do --必然可以找到
		if not tb[i] then
			tb[i] = t
			tb.num = tb.num+1
			if i > tb.max then
				tb.max = i
			end
			return t
		end
	end
	assert(false)
end

return timer_vector