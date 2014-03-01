
local sprite = class(cell,"sprite")

local g_sprite_id = 1

--ÄÃÎäÆ÷£¬ÏÈäÖÈ¾ÈË£¬ÔÙäÖÈ¾ÎäÆ÷¡£
--»»É«¡£

function sprite:create_sprite(id,x,y,z,ani_id,use_zgp)
	if not id then
		id = g_sprite_id
		g_sprite_id = id + 1
	end
	cell.create_cell(self,tostring(id),x,y,z or 0,0,0)
	local body = sprite_body()
	body:create_body("body",0,0,-1000,ani_id,use_zgp)
	self:add_child(body)

	assert(self:get_body())
end

function sprite:get_body()
	return self:find_child("body")
end

function sprite:get_ani()
	return self:get_body().ani
end

function sprite:in_rect(x,y)
	local body = self:get_body() 
	if body then 
		return body:in_rect(x,y)
	end
	return self:_in_rect(x,y)
end

function sprite:in_rect_pixel(x,y)
	local body = self:get_body() 
	if body then 
		return body:in_rect_pixel(x,y)
	end
	return self:_in_rect(x,y)
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

function sprite:do_ani(ani_name,callback)
	local body = self:get_body() 
	body:do_ani(ani_name,callback)
end

function sprite:walk_to(x,y,speed,callback)
	assertex(speed > 0 and speed < 1000,speed)

	local x0,y0 = self:get_pos()
	local path = cdriver.find_path(x0,y0,x,y)
	if not path then
		return 
	end

	if self.action then
		self.action:stop_action()
	end

	self:do_ani("walk")
	local mv = action.action_movepath()
	self.action = mv
	mv:move_path(self,path,false,speed,
		function()
			local ani = self:get_ani()
			--print("mv:move_path end")
			ani:ani_end(function()
				 --print("ani:ani_toend") 
				 self:stand() end)
			if callback then
				callback()
			end
		end)
end

function sprite:set_dir(dir)
	assertex(dir >= 0 and dir < 8,dir)
	self:get_body():set_dir(dir)
end

function sprite:get_dir()
	return self:get_body():get_dir()
end

function sprite:stand()
	self:get_body():stand()
end

function sprite:stop()
	if self.action then
		self.action:stop_action()
		self.action = nil
	end
	self:stand()
end

--function sprite:recv_mouse_msg(mouse_event,x,y,param)
--	local rt = cell.recv_mouse_msg(self,mouse_event,x,y,param)
	--if mouse_event == WM_RBUTTONDOWN then
		--print("sprite:recv_mouse_msg WM_RBUTTONDOWN",x,y,self:in_rect(x,y),self:in_rect_pixel(x,y),self.name)
	--end
--	return rt
--end

function sprite:on_get_hover(is_hover,hover)
	cell.on_get_hover(self,is_hover,hover)
	--print("sprite:on_get_hover",is_hover)
	if is_hover then
		self.shader = "light"
		self.shader_param = 0.2
	else
		--self.shader = "dark"
		self.shader = nil
	end
end


function sprite:on_mouse_msg(mouse_event,x,y,param)
	if mouse_event == WM_RBUTTONDOWN then
		--print("sprite:on_mouse_msg WM_RBUTTONDOWN",self.name)
		ui:on_sprite_mouse_rd(self,x,y)
	elseif mouse_event == WM_MOUSEWHEEL then
		self:on_mouse_scroll_room(param,0.5,2)
	end

	return self
end

function sprite:on_loaded_from_table()
end


return sprite