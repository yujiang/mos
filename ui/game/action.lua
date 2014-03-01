--action行为,no stop.

local action = class("action")
function action:add_timer()
	assert(self.timer == nil)
	self.timer = g_timer:add_timer_everyframe(self.on_timer_update,self)		
end

function action:on_end(rt)
	self.timer = nil
	if self.callback then
		self.callback(rt)
	end
end

function action:stop_action()
	if self.timer then
		self.timer:destroy()
		self.timer = nil	
	end
	self.co = nil
end

local coroutine_wrap = coroutine.create
--[[
local coroutine_wrap = function(func)
	local ok,ret = {xpcall(function() return func() end, debug.excepthook)}
	return ok and ret
end
--]]

--local coroutine_resume = coroutine.resume
local coroutine_resume = function(...)
	local ok,rt = coroutine.resume(...)
	if not ok then
		print("coroutine.resume error",rt)
		print(debug.traceback())
	end
	return ok,rt
end

function action:on_timer_update(pass)
	assert(self.co)
	local ok,rt = coroutine_resume(self.co,pass)
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

	local x_speed = (x-x0) / distance * speed
	local y_speed = (y-y0) / distance * speed

	self.co = coroutine_wrap(
		function()
			while true do
				local pass = coroutine.yield()
				local x0,y0 = cell:get_pos()
				local dx = x0 + x_speed * pass 
				local dy = y0 + y_speed * pass 
				--print ("action_move:move_to",pass,dx,dy,x,y)

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
	if cell.set_dir then --cell会变方向吗?
		local dir = cdriver.get_dir(x-x0,y-y0)
		if dir >= 0 then
			cell:set_dir(dir)
		end
	end
	return true
end

local action_ani = class(action,"action_ani")
function action_ani:do_ani(image,ani_speed,frames,loop,callback)
	--print("action_ani:do_ani",image.frame,ani_speed,frames,loop)
	assert(ani_speed > 0)
	self.callback = callback
	self.co = coroutine_wrap(
		function()
			local index = 1
			while true do
				local pass = coroutine.yield()
				local frame = frames[index]
				image:change_frame(frame)
				index = index + 1
				--print("action_ani:do_ani",image.frame,index)
				if index > #frames then
					if loop then
						index = 1
					else
						break
					end
				end
			end
			return true
		end
	)
	self.timer = g_timer:add_timer_list(ani_speed,self.on_timer_update,self)	
	return true;
end


local action_fade = class(action,"action_fade")
function action_fade:fade(cell,alpha_start,alpha_end,speed,callback)
	self.callback = callback
	assert(speed ~= 0)
	cell.alpha = alpha_start
	
	self.co = coroutine_wrap(
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
	
	self.co = coroutine_wrap(
		function()
			while true do
				local pass = coroutine.yield()
				local c = cell:clone()
				c.name = cell.name.."_shadow_"..index
				c:disable()
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
	
	self.co = coroutine_wrap(
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

	self.co = coroutine_wrap(
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

local action_movepath = class(action,"action_movepath")
function action_movepath:move_path(sprite,paths,loop,speed,callback)
	assert(speed ~= 0)
	assert(#paths >= 2)
	--print("action_path:path",#paths)
	local co 
	co = coroutine_wrap(
		function()
			local index = 1
			while true do
				local x,y = paths[2*index-1],paths[2*index]
				--print("action_path:path",index,pos[1],pos[2])
				local mv = action_move()
				mv:move_to(sprite,x,y,speed,
					function() 
						coroutine_resume(co) 
					end)
				self.move = mv
				coroutine.yield()
				index = index + 1
				if 2*index > #paths then
					if not loop then
						break
					else
						index = 1
					end
				end
			end
			if callback then
				callback()
			end
			return true
		end
	)
	coroutine_resume(co)
end

function action_movepath:stop_action()
	action.stop_action(self)
	if self.move then
		self.move:stop_action()
	end
end

local action_moverandom = class(action,"action_moverandom")
function action_moverandom:move_random(sprite,l,t,r,b,speed,sleepmin,sleepmax)
	assert(speed > 0)
	--print("action_path:path",#paths)
	local co 
	co = coroutine_wrap(
		function()
			while true do
				local x = math.random(l,r)
				local y = math.random(t,b)
				local mv = action_move()
				mv:move_to(sprite,x,y,speed,
					function() 
						if not sleepmin then
							coroutine_resume(self.co) 
						else
							local sleep = math.random(sleepmin,sleepmax)
							g_timer:add_timer(sleep/1000,function() coroutine_resume(co) end)
						end
					end)
				self.move = mv
				coroutine.yield()
			end
		end
	)
	coroutine_resume(co)
end

function action_moverandom:stop_action()
	action.stop_action(self)
	if self.move then
		self.move:stop_action()
	end
end


return {
	action_move = action_move,
	action_ani = action_ani,
	action_fade = action_fade,
	action_shadow = action_shadow,
	action_room = action_room,
	action_rotate = action_rotate,
	action_movepath =  action_movepath,
	action_moverandom =  action_moverandom,
}
