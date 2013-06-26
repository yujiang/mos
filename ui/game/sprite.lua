
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
	--self.dir = 0
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

function sprite:get_bg_ani()
	return self:get_bg().ani
end

function sprite:do_ani(ani_name)
	local tb = g_ani_data:find_ani_data(self.ani_id,ani_name)	
	if not tb then
		print("error! sprite:do_ani ",self.ani_id,ani_name)
		return
	end
	
	if not self:get_bg() then
		self:set_bg(tb.image_file,tb.frame_start)
	end

	local bg = self:get_bg() 
	--print("do_ani bg",bg)
	bg:set_ani_tb(tb)
end

function sprite:get_move()
	if not self.move then
		self.move = move()
		self.move:create_move(self)
		self.move.on_reached = function(is_reached)
			--print("sprite.move.on_reached",self.name,is_reached)
			if is_reached then	--否则可能是上一个的退出。
				self:stand() 
				--需要把ani完成，快速播放完成，之后再stand
				--效果并不好，所以去掉了。
--				local tb = g_ani_data:find_ani_data(self.ani_id,"walkend")
--				if tb and false then
--					local ani = self:get_bg().ani
--					ani:set_ani_tb(tb,false)
--					ani.on_aniend = function(ended)
--						local tb = g_ani_data:find_ani_data(self.ani_id,"walkend2")
--						ani:set_ani_tb(tb,false)
--						ani.on_aniend = function(ended)
--							self:do_ani("stand")
--							ani.on_aniend = nil
--						end
--					end
--				else
--					self:do_ani("stand")
--				end
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
	local bg = self:get_bg() 
	if bg then
		--print("sprite:set_dir",dir)
		bg.ani:set_ani_dir(dir)
	end
end

function sprite:on_timer_stand()
	print("sprite:on_timer_stand()")
	local ani = self:get_bg_ani()
	if ani.ani_tb.name == "stand" then
		ani.ani_tb = nil
		self:stand()
	end
end

function sprite:stand()
	self:do_ani("stand")
	local ani = self:get_bg_ani()
	ani.on_aniend = 
		function(ended)
			--add a random timer to do again
			local tm = math.random(1,100) / 10 + 3
			g_timer:add_timer(tm,self.on_timer_stand,self)
		end
	return false
end

function sprite:stop()
	self:stand()
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