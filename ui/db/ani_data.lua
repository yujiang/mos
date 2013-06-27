--动画

local ani_data = class("ani_data")

function ani_data:create_ani_data()
	self.shapes = {}
end

function ani_data:regist_ani_data_simple(shape,name,image_file,ani_speed,frame_start,frame_end,loop)
	return self:regist_ani_data(shape,name,image_file,ani_speed,1,frame_end-frame_start,frame_start,frame_end,1,loop,false)
end

function ani_data:regist_ani_data(shape,name,image_file,ani_speed,dir,dir_frame,frame_start,frame_end,frame_step,loop,dir_inverse)
	self.shapes[shape] = self.shapes[shape] or {}
	local t = {
		shape = shape,
		name = name,
		image_file = image_file,
		ani_speed = ani_speed,
		dir = dir, --方向数目。
		dir_frame = dir_frame, --每个方向的帧数。
		frame_start = frame_start,
		frame_end = frame_end,
		frame_step = frame_step,
		loop = loop,
		dir_inverse = dir_inverse,
		--cx = cx,
		--cy = cy,
	}
	self.shapes[shape][name] = t
	return t
end

function ani_data:find_ani_data_raw(shape,name)
	if self.shapes[shape] then
		return self.shapes[shape][name]
	end
end

local default_char_ani = 
{
	walk = {file = "walk",speed = 0.1,loop = true},
	fastwalk = {file = "walk",speed = 0.05,loop = true},
	stand = {file = "stand",speed = 0.3,loop = false},
}

function ani_data:find_ani_data(shape,name,use_zgp)
	local tb = self:find_ani_data_raw(shape,name)
	--print(tb,use_zgp)
	if not tb and use_zgp then
		local default = default_char_ani[name] or default_char_ani.stand		
		local rt,dir,frame 
		local s = string.format("char/%04d/%s.zgp",shape,default.file)
		rt,dir,frame = cdriver.regist_zgp(s)
		--print(rt,dir,frame)
		if not rt then
			return
		end
		return self:regist_ani_data(shape,name,s,default.speed,dir,frame,0,frame,1,default.loop,true)
	end
	return tb
end

return ani_data
