--¶¯»­

local ani = class("ani")

function ani:create_ani(image)
	self.image = image
end

function ani:set_ani(ani_speed,frame_start,frame_end,loop,step)
	self.speed = ani_speed
	self.frame_start = frame_start 
	self.frame_end = frame_end
	self.loop = loop
	self.step = step or 1 --may be -1...

	self:add_timer()
end

function ani:stop_ani()
	if self.timer then
		self.timer.invalid = true
		self.timer = nil
	end
end

function ani:add_timer()
	self:stop_ani()	
	self.timer = g_timer:add_timer(self.speed,ani.on_timer_updateframe,self)
end

function ani:on_loaded_from_table()
	self:add_timer()
end

function ani:on_timer_updateframe()
	local img = self.image
	--print("ani:on_timer_updateframe()",img.frame)
	img.frame = img.frame + self.step
	if img.frame >= self.frame_end then
		if self.loop then
			img.frame = self.frame_start
		else
			img.frame = self.frame_end - 1
			return false
		end
	elseif img.frame < self.frame_start then
		if self.loop then
			img.frame = self.frame_end - 1
		else
			img.frame = self.frame_start
			return false
		end		
	end
	return true
end

return ani
