--action行为,no stop.

local action = class("action")
function action:add_timer()
	self.timer = g_timer:add_timer_everyframe(self.on_timer_update,self)		
end

function action:on_end(rt)
	self.timer = nil
	if self.callback then
		self.callback(rt)
	end
end

function action:destroy()
	self.timer:destroy()
	self.timer = nil	
	self.co = nil
end

function action:on_timer_update(pass)
	assert(self.co)
	local ok,rt = coroutine.resume(self.co,pass)
	if not ok then
		self:on_end(false)
	elseif rt then
		self:on_end(true)
	else
		return true
	end
end


local action_move = class(action,"action_move")
--speed 每秒多少像素？
function action_move:move_to(cell,x,y,speed,callback)
	assert(speed > 0)
	self.callback = callback

	local x0,y0 = cell:get_pos()
	local distance = math.sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))
	if distance <= 0.01 then
		self:on_end()
		return 
	end

	local x_speed = (x-x0) / distance * speed
	local y_speed = (y-y0) / distance * speed

	self.co = coroutine.create(
		function()
			while true do
				local pass = coroutine.yield()
				local x0,y0 = cell:get_pos()
				local dx = x0 + speed * pass 
				local dy = y0 + speed * pass 
				--print ("action_move:move_to",pass,dx,dy)

				if dx == x and dy == y or
					(dx - x) * (x0 - x) < 0 or 
					(dy - y) * (y0 - y) < 0 then
					cell:set_pos(x,y)	
					break
				end
				cell:set_pos(dx,dy)	
			end
			return true
		end
	)

	self:add_timer()
	return true
end

local action_fade = class(action,"action_fade")
function action_fade:fade(cell,alpha_start,alpha_end,speed,callback)
	self.callback = callback
	assert(speed ~= 0)
	cell.alpha = alpha_start
	
	self.co = coroutine.create(
		function()
			while true do
				local pass = coroutine.yield()
				local a = pass * speed 
				--print("action_fade:fade",cell.alpha,a)
				cell.alpha = cell.alpha + a
				if speed > 0 and cell.alpha > alpha_end or
					speed < 0 and cell.alpha < alpha_end then
					cell.alpha = self.alpha_end
					break
				end
			end
			return true
		end
	)

	self:add_timer()
	return true
end


local action_shadow = class(action,"action_shadow")

function action_shadow:shadow(cell,time_once,alpha_start,alpha_end,alpha_speed)
	local index = 1
	local f = cell.father
	assert(f)
	
	self.co = coroutine.create(
		function()
			while true do
				local pass = coroutine.yield()
				local c = cell:clone()
				c.name = "shadow_"..index
				index = index + 1
				f:add_child(c)

				local mv = action_fade()
				mv:fade(c,alpha_start,alpha_end,alpha_speed,
					function() 
						f:remove_child(c)
					end)
			end
			return true
		end
	)

	self.timer = g_timer:add_timer_list(time_once,self.on_timer_update,self)	
	self:on_timer_update()
	return true		
end

local action_room = class(action,"action_room")

function action_room:room(cell,room_start,room_end,speed,callback)
	self.callback = callback
	assert(speed ~= 0)
	cell.room = room_start

	--print("action_room:room")
	
	self.co = coroutine.create(
		function()
			while true do
				local pass = coroutine.yield()
				local a = pass * speed 
				--print("action_room:room()",cell.room,a)
				cell.room = cell.room + a
				if speed > 0 and cell.room > room_end or
					speed < 0 and cell.room < room_end then
					cell.room = room_end
					break
				end
			end
			return true
		end
	)

	self:add_timer()
	return true	
end

local action_rotate = class(action,"action_rotate")
function action_rotate:rotate(sprite,time_once)
	assert(speed ~= 0)
	local dir = sprite:get_dir()
	assert(dir)

	self.co = coroutine.create(
		function()
			while true do
				local pass = coroutine.yield()
				--print("action_rotate:rotate",dir)
				dir = dir + 1
				if dir >= 8 then
					dir = 0
				end
				sprite:set_dir(dir)
			end
			return true
		end
	)

	self.timer = g_timer:add_timer_list(time_once,self.on_timer_update,self)	
	return true		
end

local action_ani = class(action,"action_ani")
function action_ani:ani(sprite,frames,loop,callback)
	self.callback = callback
	
end

return {action_move = action_move,
	action_fade = action_fade,
	action_shadow = action_shadow,
	action_room = action_room,
	action_rotate = action_rotate,
	action_ani = action_ani,}
