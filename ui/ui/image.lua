--根据图形文件来重建image

local image = class(cell,"image")

function image:create_image(name,x,y,z,image_file,frame)
	self:set_image(image_file,frame)
	cell.create_cell(self,name,x,y,z,self.w,self.h)
	self._disable = true;
end

function image:set_image(image_file,frame)
	self.image_file = image_file
	self.frame = frame 
	local w,h,cx,cy = cdriver.get_image_sizecg(image_file,frame) --从文件得到w,h,cx,cy
	self.w = w
	self.h = h
	self.cx = cx
	self.cy = cy
end

function image:get_render_override()
	--print("image:get_render_override()",self.name)
	if self.image_file then
		return self
	end
end

--设定ani
function image:set_ani(ani_speed,frame_start,frame_to,loop)
	--print("image:get_render_override()",self.name)
	if not self.ani then
		self.ani = ani()
		self.ani:create_ani(self)
	end
	self.ani:set_ani(ani_speed,frame_start,frame_to,loop)
end

function image:on_loaded_from_table()
	if self.ani then
		setmetatable(self.ani,ani)
		self.ani:on_loaded_from_table()
	end
end

function image:on_destroyed()
	--cell.on_destroyed(self)
	if self.ani then
		self.ani:stop_ani()
	end
end

function image:is_jpg()
	--print(self.name,self.image_file)
	return string.sub(self.image_file,#self.image_file - 3) == ".jpg"
end

function image:get_rect()
	return self.x - self.cx,self.y - self.cy,self.x + self.w - self.cx,self.y + self.h - self.cy
end

function image:in_rect_pixel(x,y)
	if not self:in_rect(x,y) then
		return false
	end
	if  self:is_jpg() then --jpg没有alpha通道
		return true
	else
		return cdriver.in_image(self.image_file,self.frame,x + self.cx - self.x,y + self.cy - self.y)
	end
end

function image:on_reload_class(old,new)
	--print("image:on_reload_class(old,new)")
	if getmetatable(self.ani) == old then
		setmetatable(self.ani,new)
	end
end

return image