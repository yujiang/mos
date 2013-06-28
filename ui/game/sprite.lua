
local sprite = class(cell,"sprite")

local g_sprite_id = 1

--拿武器，先渲染人，再渲染武器。
--换色。

function sprite:create_sprite(id,x,y,z,ani_id,use_zgp)
	if not id then
		id = g_sprite_id
		g_sprite_id = id + 1
	end
	cell.create_cell(self,tostring(id),x,y,z or 0,0,0)
	--self.ani_id = ani_id
	self.emitter = emitter()
	--self.dir = 0
	--self.use_zgp = use_zgp

	self:get_move()

	local body = sprite_body()
	body:create_body("body",0,0,-1000,ani_id,use_zgp)
	self:add_child(body)

	assert(self:get_body())
end

function sprite:get_body()
	return self:find_child("body")
end

function sprite:set_weapon(weapon_id)
	self:get_body():set_weapon(weapon_id)
end

function sprite:in_rect(x,y,room)
	local body = self:get_body() 
	if body then 
		return body:in_rect(x-self.x,y-self.y,room)
	end
	return self:_in_rect(x,y,room)
end

function sprite:in_rect_pixel(x,y,room)
	local body = self:get_body() 
	if body then 
		return body:in_rect_pixel(x-self.x,y-self.y,room)
	end
	return self:_in_rect(x,y,room)
end


function sprite:set_name(name,color)
	name = name or "sprite_"..self.name
	self:set_caption(name)
	local cap = self:get_caption()
	local text = self:get_caption_text()
	cap.w = text.text_w
	cap.h = text.text_h
	cap.x = -cap.w / 2
	if color then
		text:set_color(color)
	end
end

function sprite:get_z()
	return self.y + self.x / 10000
end

function sprite:do_ani(ani_name)
	local body = self:get_body() 
	body:do_ani(ani_name)
end

function sprite:get_move()
	if not self.move then
		self.move = move()
		self.move:create_move(self)
		self.move.on_reached = function(is_reached)
			--print("sprite.move.on_reached",self.name,is_reached)
			if is_reached then	--否则可能是上一个的退出。
				self:stand() 
			end
			self.emitter:emit("reach",is_reached)
		end
	end
	return self.move
end

function sprite:get_ai()
	if not self.ai then
		self.ai = ai()
		self.ai:create_ai(self)
	end
	return self.ai
end

function sprite:walk_to(x,y,speed)
	assert(speed > 0)
	self:do_ani("walk")
	local m = self:get_move()
	local dir = m:move_to(x,y,speed)
	if dir then
		self:set_dir(dir)
	end
end

function sprite:set_dir(dir)
	self:get_body():set_dir(dir)
end

function sprite:stand()
	self:get_body():stand()
end

function sprite:stop()
	self:stand()
	if self.move then
		self.move:stop_move()
	end
end

--function sprite:recv_mouse_msg(mouse_event,x,y,param,room)
--	local rt = cell.recv_mouse_msg(self,mouse_event,x,y,param,room)
	--if mouse_event == WM_RBUTTONDOWN then
		--print("sprite:recv_mouse_msg WM_RBUTTONDOWN",x,y,self:in_rect(x,y,room),self:in_rect_pixel(x,y,room),self.name)
	--end
--	return rt
--end

function sprite:on_mouse_msg(mouse_event,x,y,param)
	if mouse_event == WM_RBUTTONDOWN then
		--print("sprite:on_mouse_msg WM_RBUTTONDOWN",self.name)
		ui:on_sprite_mouse_rd(self,x,y)
	end
	return self
end

function sprite:on_reload_class(old,new)
	if self.ai and getmetatable(self.ai) == old then
		setmetatable(self.ai,new)
	end
end

function sprite:on_loaded_from_table()
	self.emitter = emitter()
end


return sprite