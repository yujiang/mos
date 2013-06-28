--动画

local ani = class("ani")

function ani:create_ani(image)
	self.image = image
	self.ani_dir = 0
end

--sprite始终用8方向，等于dirmax的几方向呢？
--并且由于历史的原因方向数目跟我们想的不一致。
local magic_dirs = {
	--0,1,2,3,4,5,6,7,
	6,3,7,0,4,1,5,2,
}

function dir8_inverse(dir,dirmax)
	if dirmax == 8 then 
		return magic_dirs[dir+1]
	elseif dirmax == 1 then 
		return 0
	elseif dirmax == 4 then  
		--未实现。
	end
end

function dir8(dir,dirmax)
	if dirmax == 8 then 
		return dir
	elseif dirmax == 1 then 
		return 0
	elseif dirmax == 4 then  
		--未实现。
	end
end

--复杂的ani
function get_dir_base(tb,dir)
	return tb.dir_inverse and dir8_inverse(dir,tb.dir) or dir8(dir,tb.dir)
end

function ani:set_ani_tb(tb,updateimage)
	if self.ani_tb == tb then
		return
	end
	self.ani_tb = tb
	local base = get_dir_base(tb,self.ani_dir) * tb.dir_frame
	--有可能不需要set_image因为从可能从walk到快速walk。
	--print("ani:set_ani_tb0",tb.image_file)
	--print("ani:set_ani_tb1 self.ani_dir "..self.ani_dir.." tb.dir " .. tb.dir .. " base ".. base .. " tb.frame_start " ..tb.frame_start)
	
	if updateimage then
		self.image:set_image(tb.image_file,base + tb.frame_start)
	end

	if tb.ani_speed > 0 then
		self:set_ani(tb.ani_speed,base + tb.frame_start,base + tb.frame_end,tb.loop)
	else
		self:stop_ani()
	end
	self.on_aniend = nil
end

function ani:set_ani_dir(dir)
	if self.ani_dir == dir then
		return
	end
	self.ani_dir = dir
	if self.ani_tb then
		local tb = self.ani_tb
		local base = get_dir_base(tb,self.ani_dir) * tb.dir_frame
		local off = self.image.frame - self.frame_start
		self.image:change_frame(base + off + tb.frame_start)
		--print("ani:set_ani_dir dir frame",dir,base + off)
		self.frame_start = base + tb.frame_start
		self.frame_end = base + tb.frame_end
	end
end

--简单的ani
function ani:set_ani(ani_speed,frame_start,frame_end,loop,step)
	self.speed = ani_speed
	self.frame_start = frame_start 
	self.frame_end = frame_end
	self.loop = loop
	self.step = step or 1 --may be -1...

	self:add_timer()
end

function ani:stop_ani()
	self:stop_timer()
end

function ani:stop_timer()
	if self.timer then
		self.timer.invalid = true
		self.timer = nil
	end
end

function ani:add_timer()
	self:stop_timer()	
	self.timer = g_timer:add_timer(self.speed,ani.on_timer_updateframe,self)
end

function ani:on_loaded_from_table()
	self:add_timer()
end

function ani:_on_timer_updateframe()
	local img = self.image
	--print("ani:on_timer_updateframe()",img.frame)
	frame = img.frame + self.step
	if frame >= self.frame_end then
		if self.loop then
			frame = self.frame_start
		else
			frame = self.frame_end - 1
			if self.on_aniend then
				self.on_aniend(true)
			end
			return false,frame
		end
	elseif frame < self.frame_start then --倒带。
		if self.loop then
			frame = self.frame_end - 1
		else
			frame = self.frame_start
			if self.on_aniend then
				self.on_aniend(true)
			end
			return false,frame
		end		
	end
	return true,frame
end

function ani:on_timer_updateframe()
	local rt , frame = self:_on_timer_updateframe()
	self.image:change_frame(frame)
	return rt
end

return ani
