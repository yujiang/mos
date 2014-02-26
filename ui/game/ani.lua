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
	self.callback = nil
	if self.ani_tb == tb then --同一个ani
		self.loop = tb.loop
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

		if off < 0 then
			off = 0
		elseif off >= tb.dir_frame then
			off = tb.dir_frame-1
		end
		--print("set_ani_dir",base,off,tb.frame_start)
		self.image:change_frame(base + off + tb.frame_start)
		--print("ani:set_ani_dir dir frame",dir,base + off)
		self.frame_start = base + tb.frame_start
		self.frame_end = base + tb.frame_end
	end
end

--简单的ani with dir
function ani:set_ani(ani_speed,frame_start,frame_end,loop,step,callback)
	--print(frame_start,frame_end)
	self.speed = ani_speed
	self.frame_start = frame_start 
	self.frame_end = frame_end
	self.loop = loop
	self.step = step or 1 --may be -1...
	self.callback = callback
	self:add_timer()
	self.ani_type = 1
end

function ani:custom_ani(ani_speed,frames,loop,step,callback)
	self.speed = ani_speed
	self.custom_frames = frames
	self.custom_index = 1
	self.callback = callback
	self:add_timer()
	self.ani_type = 2
end

function ani:stop_ani()
	self:stop_timer()
end

function ani:stop_timer()
	if self.timer then
		self.timer:destroy()
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

function ani:ontimer_updatecustom()
	local img = self.image
	local rt = true
	if self.custom_index <= #self.custom_frames then
	else
		if self.loop then
			self.custom_index = 1
		else
			self.custom_index = #self.custom_frames
			rt = false
		end
	end
	frame = self.custom_frames[self.custom_index]
	self.custom_index = self.custom_index + 1
	return rt,frame
end

--此处不用coroutine是因为太多了，不像action，基本只有一两个。
function ani:ontimer_updatestartend()
	local img = self.image
	--print("ani:on_timer_updateframe()",img.frame)
	local frame = img.frame + self.step
	if frame > self.frame_end then
		if self.loop then
			frame = self.frame_start
		else
			frame = self.frame_end 
			return false,frame
		end
	elseif frame < self.frame_start then --倒带。
		if self.loop then
			frame = self.frame_end 
		else
			frame = self.frame_start
			return false,frame
		end		
	end
	return true,frame
end

function ani:on_timer_updateframe()
	local funcs = {nil,ani.ontimer_updatecustom}
	local func = funcs[self.ani_type] or ani.ontimer_updatestartend
	local rt , frame = func(self) 
	if not rt then
		if self.callback then
			self.callback(true)
		end
	end
	self.image:change_frame(frame)
	return rt
end

--会冲突出现站着移动的情况，所以callback不能指定必须传递。
function ani:ani_aniend(callback)
	local frame = self.image.frame
	local a = frame - self.frame_start
	if a == 0 then
		callback()
		return false
	end
	self.loop = false
	self.callback = callback
	return true
end



return ani
