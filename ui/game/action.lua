--action行为,no stop.

local action = class("action")
function action:add_timer()
	self.timer = g_timer:add_timer_everyframe(self.on_timer_update,self)		
end

function action:on_end()
	self.timer = nil
	if self.callback then
		self.callback()
	end
end

function action:destroy()
	self.timer:destroy()
	self.cell = nil
	self.timer = nil	
end

local action_move = class(action,"action_move")
--speed 每秒多少像素？
function action_move:move_to(cell,x,y,speed,callback)
	self.cell = cell
	self.callback = callback

	assert(speed > 0)
	local x0,y0 = self.cell:get_pos()

	local distance = math.sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))
	if distance <= 0.0001 then
		self:on_end()
		return 
	end

	self.x_start = x0
	self.y_start = y0
	self.x_des = x
	self.y_des = y
	self.x_speed = (x-x0) / distance * speed
	self.y_speed = (y-y0) / distance * speed

	--print("move:move_to",x,y,speed,self.x_speed,self.y_speed)

	self:add_timer()
	local dir = cdriver.get_dir(x-x0,y-y0)
	--self.on_change_dir(dir)
	return dir
end

function action_move:on_timer_update(pass)
	local x,y = self.cell:get_pos()
	local dx = x + self.x_speed * pass 
	local dy = y + self.y_speed * pass 
	--print ("move:on_timer_update",pass,dx,dy)

	if dx == self.x_des and dy == self.y_des or
		(dx - self.x_des) * (x - self.x_des) < 0 or 
		(dy - self.y_des) * (y - self.y_des) < 0 then
		self.cell:set_pos(self.x_des,self.y_des)	
		self:on_end()
		return
	else
		self.cell:set_pos(dx,dy)	
		return true
	end
end

local action_fade = class(action,"action_fade")
function action_fade:fade(cell,alpha_start,alpha_end,speed,callback)
	self.cell = cell
	self.callback = callback

	assert(speed ~= 0)
	
	self.alpha_start = alpha_start
	self.cell.alpha = alpha_start
	self.alpha_end = alpha_end
	self.speed = speed

	self:add_timer()
end

function action_fade:on_timer_update(pass)
	local a = pass * self.speed 
	--print("action_fade:on_timer_update(pass)",self.cell.alpha,a)
	self.cell.alpha = self.cell.alpha + a
	if self.speed > 0 and self.cell.alpha > self.alpha_end or
		self.speed < 0 and self.cell.alpha < self.alpha_end then
		self.cell.alpha = self.alpha_end
		self:on_end()		
		return 
	end
	return true
end

local action_shadow = class(action,"action_shadow")

function action_shadow:shadow(cell,time_once,alpha_start,alpha_end,alpha_speed)
	self.cell = cell
	--self.cells = {}
	self.alpha_start = alpha_start
	self.alpha_end = alpha_end
	self.alpha_speed = alpha_speed
	self.index = 1
	self:on_timer_update()
	self.timer = g_timer:add_timer_list(time_once,self.on_timer_update,self)		
end

function action_shadow:on_timer_update()
	--print("action_shadow:on_timer_update")
	local c = self.cell:clone()
	--table.insert(self.cells,c)
	c.name = "shadow_"..self.index
	self.index = self.index + 1

	local f = self.cell.father
	assert(f)
	f:add_child(c)

	local mv = action_fade()
	mv:fade(c,self.alpha_start,self.alpha_end,self.alpha_speed,
		function() 
			f:remove_child(c)
		end)
	return true
end

function action:rotate(cell,angle_start,angle_rotate,speed,callback)
end

return {action_move = action_move,action_fade = action_fade,action_shadow = action_shadow}
