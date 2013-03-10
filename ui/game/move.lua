--运动
local move= class("move")

function move:create_move(cell)
	self.cell = cell
end

function move:stop_move()
	if self.timer then
		self.timer.invalid = true
		self.timer = nil
		self.on_reached(false)
	end
end

function move:on_reached(is_reached)
	
end

function move:on_timer_updatepos(pass)
	local x,y = self.cell:get_pos()
	local dx = x + self.x_speed * pass 
	local dy = y + self.y_speed * pass 
	--print ("move:on_timer_updatepos",pass,dx,dy)

	if dx == self.x_des and dy == self.y_des or
		(dx - self.x_des) * (x - self.x_des) < 0 or 
		(dy - self.y_des) * (y - self.y_des) < 0 then
		self.cell:set_pos(self.x_des,self.y_des)	
		--return nil 到了，不再执行timer!
		self.timer = nil
		self.on_reached(true)
	else
		self.cell:set_pos(dx,dy)	
		return true
	end
end

function move:add_timer()
	self:stop_move()
	self.timer = g_timer:add_timer_everyframe(move.on_timer_updatepos,self)		
end

--speed 每秒多少像素？
function move:move_to(x,y,speed)
	assert(speed > 0)
	local x0,y0 = self.cell:get_pos()

	local distance = math.sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))
	if distance <= 0.0001 then
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
end

--支持一个coroutine的方法。
return move