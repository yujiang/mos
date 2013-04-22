
local timer = class("timer")

TIMER_TYPE_MAP = 1
TIMER_TYPE_LIST = 2
TIMER_TYPE_VECTOR = 3
TIMER_TYPE_EVERYFRAME = 4

function timer:create_timer()
	--print("timer:create_timer()",self)
	self.map = timer_map()
	self.map:create_timer_map()

	self.list = timer_list()
	self.list:create_timer_list()

	self.vector = timer_vector()	
	self.vector:create_timer_vector()

	self.everyframe = timer_everyframe()
	self.everyframe:create_timer_everyframe()

	self.pause_add_timer = {}

	self.time_clock = os.clock()

	self.fps = 0
	self.fps_count = 0
	self.time_fps = math.floor(os.clock())
end

function timer:get_fps()
	return self.fps
end

--type 1,2,3,
function timer:_add_timer(t)
	--print("timer:_add_timer()",self)
	local type = t.type
	local time = t.time
	if type == TIMER_TYPE_LIST then
		t.clock_time = os.clock() + time
		self.list:add_timer(time,t)
	elseif type == TIMER_TYPE_VECTOR then
		self.vector:add_timer(time,t)
	elseif type == TIMER_TYPE_EVERYFRAME then
		self.everyframe:add_timer(t)
	else --default is TIMER_TYPE_MAP
		t.clock_time = os.clock() + time
		self.map:add_timer(time,t)
	end
	return t
end

function timer:add_timer_type(time,type,func,param)
	local t = {time = time,func = func,param = param,type = type}
	if self.in_on_every_frame then
		table.insert(self.pause_add_timer,t)		
	else
		self:_add_timer(t)
	end
	return t
end

function timer:add_timer_everyframe(func,param)
	return self:add_timer_type(0,TIMER_TYPE_EVERYFRAME,func,param)
end

function timer:add_timer(time,func,param)
	return self:add_timer_type(time,TIMER_TYPE_MAP,func,param)
end


function timer:on_every_frame()
	self.in_on_every_frame = true
	
	local clock = os.clock() - self.time_clock
	self.time_clock = os.clock()

	if self.time_clock - self.time_fps >= 1 then
		self.time_fps = math.floor(self.time_clock)
		self.fps = self.fps_count
		self.fps_count = 0
	else
		self.fps_count = self.fps_count + 1
	end

	self.map:on_every_frame()
	self.list:on_every_frame()
	self.vector:on_every_frame(clock)
	self.everyframe:on_every_frame(clock)

	self.in_on_every_frame = false
	
	for k,v in ipairs(self.pause_add_timer) do
		self:_add_timer(v)		
	end	
	self.pause_add_timer = {}
end

return timer