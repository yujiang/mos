--运动
local move= class("move")

function move:create_move(cell)
	self.cell = cell
end

function move:stop_move()
	if self.timer then
		self.timer:destroy()
		self.timer = nil
		self.on_reached(false)
	end
end

function move:on_reached(is_reached)
	
end

function move:on_change_dir(dir)
	
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
		--self.timer = nil

		--print("move.on_reach,",self.path,#self.path)
		if not self.path or #self.path <= self.pos or not self:continue_move() then
			self.timer = nil
			self.on_reached(true)
		else
			return true
		end
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
function move:move_to_straight(x,y,speed)
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
	local dir = cdriver.get_dir(x-x0,y-y0)
	self.on_change_dir(dir)
	return dir
end

function move:move_to(x,y,speed)
	self:stop_move()

	local x0,y0 = self.cell:get_pos()
	local path = cdriver.find_path(x0,y0,x,y)
	if not path then
		return 
	end

	local function print_path()
		local s = ""
		for i= 1,#path/2,1 do
			local nx = path[i*2-1]
			local ny = path[i*2]
			s = s .. "("..nx..","..ny..") "
		end
		print("move:move_to",x0,y0,"-->",x,y)
		print("path "..#path,s)
	end
	--print_path()

	self.speed = speed
	self.path = path
	self.pos = 0

	assert(#path >= 2)
	return self:continue_move()
end

function move:continue_move()
	local path = self.path
	local nx,ny = path[self.pos+1],path[self.pos+2]
	--print("move:continue_move()",#path,nx,ny)
	self.pos = self.pos + 2
	return self:move_to_straight(nx,ny,self.speed)
end

--支持一个coroutine的方法。
return move