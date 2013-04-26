
local sprite = class(cell,"sprite")

local g_sprite_id = 1

function sprite:create_sprite(id,x,y,z,ani_id)
	if not id then
		id = g_sprite_id
		g_sprite_id = id + 1
	end
	cell.create_cell(self,tostring(id),x,y,z or 0,0,0)
	self.ani_id = ani_id
	self.emitter = emitter()

	self:get_move()
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
	local tb = g_ani_data:find_ani_data(self.ani_id,ani_name)	
	if not tb then
		print("error! sprite:do_ani ",self.ani_id,ani_name)
		return
	end
	--self:set_center(tb.cx,tb.cy)
	self:set_bg(tb.image_file,tb.frame_start)

	local bg = self:get_bg() 
	bg.x = -tb.cx
	bg.y = -tb.cy
	
	--print(ani_name,tb.image_file,tb.frame_start,tb.ani_speed)
	--print(debug.traceback())

	if tb.ani_speed > 0 then
		bg:set_ani(tb.ani_speed,tb.frame_start,tb.frame_end,tb.loop)
	else	
		if bg.ani then
			bg.ani:stop_ani()
		end
	end
end

function sprite:get_move()
	if not self.move then
		self.move = move()
		self.move:create_move(self)
		self.move.on_reached = function(is_reached)
			--print("sprite.move.on_reached",self.name,is_reached)
			if is_reached then	--否则可能是上一个的退出。
				self:do_ani("stand")
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
	m:move_to(x,y,speed)
end

function sprite:stop()
	self:do_ani("stand")
	if self.move then
		self.move:stop_move()
	end
end

function sprite:on_mouse_msg(mouse_event,x,y,param)
	if mouse_event == WM_RBUTTONDOWN then
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