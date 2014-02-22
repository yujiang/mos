--µØÍ¼

local camera = class("camera")

function camera:create_camera(map,type)
	self.map = map
	if type == 1 then
	else --default
		self:run_normal()
	end
end

function camera:ontimer_everyframe() 
	local player = g_root:get_play()
	if player then
		local x,y = player:get_pos()
		self.map:center_play(x,y,g_root.w,g_root.h)
	end
	return true
end

function camera:run_normal() 
	if self.timer then
		self.timer:destroy()
	end
	self.timer = g_timer:add_timer_everyframe(self.ontimer_everyframe,self)
end


return camera