
local sprite_body = class(image,"sprite_body")

function sprite_body:create_body(name,x,y,z,ani_id,use_zgp)
	--self:set_image(image_file,frame)
	--print("sprite_body:create_body",name,x,y,z)
	cell.create_cell(self,name,x,y,z,0,0)
	self.ani_id = ani_id
	self.ani_id2 = ani_id2
	self.use_zgp = use_zgp
	self._disable = true

	self.need_mask = true

	self.ani = ani()
	self.ani:create_ani(self)
end

function sprite_body:change_frame(frame)
	image.change_frame(self,frame)
	if self:get_weapon() then
		self:get_weapon():change_frame(frame)
	end
end

function sprite_body:set_weapon_image()
	local w = self:get_weapon()
	if w and self.image_file then
		--从image_file来才对；self.ani_name可能是个别名并不等于文件名。
		local i,j = string.rfind(self.image_file,"/")
		--print(image_file,i,j)
		local snum = string.format("%02d/",w.weapon_id)
		local s = string.sub(self.image_file,1,i) .. snum .. string.sub(self.image_file,i+1)
		--print("sprite_body:set_image weapon",s)
		--local s = string.format("char/%04d/%02d/%s.zgp",self.ani_id,w.weapon_id,self.ani_name)
		w:set_image(s,self.frame)
	end
end

function sprite_body:set_image(image_file,frame)
	--print("sprite_body:set_image ",image_file,frame)
	image.set_image(self,image_file,frame)
	--print("image.set_image",image_file,frame)
	self:set_weapon_image()
end

function sprite_body:get_weapon()
	return self:find_child("weapon")
end

function sprite_body:set_weapon(weapon_id)
	local weapon = image()
	weapon:create_cell("weapon",0,0,1,0,0)
	weapon.need_mask = true

	self:add_child(weapon)
	weapon.weapon_id = weapon_id
	--assert(self:get_weapon())	
	self:set_weapon_image()
end

--only zgp valid
function sprite_body:clear_zgp_part_pal(part)
	self["part"..part] = nil
end

function sprite_body:reset_zgp_pal()
	for part=1,6 do 
		self["part"..part] = nil
	end
end

function sprite_body:set_zgp_part_pal(part,h,s,v)
	--local index = cdirver.create_zgp_pal_hsv(image_file,part,h,s,v)
	assert(part >= 0)

	--h -180 180  360
	--s -100 100  200
	--v -20 20    40

	self["part"..part] = (h+180) * 256 * 64 + (s+100) * 64 + (v+20)
end

function sprite_body:do_ani(ani_name)
	self.ani_name = ani_name
	local tb = g_ani_data:find_ani_data(self.ani_id,ani_name,self.use_zgp)	
	if not tb then
		print("error! sprite:do_ani ",self.ani_id,ani_name)
		assert(tb)
		return
	end
	
	self.ani:set_ani_tb(tb,true)
end

function sprite_body:on_timer_stand()
	--print("sprite:on_timer_stand()")
	local ani = self.ani
	if ani.ani_tb and ani.ani_tb.name == "stand" then
		ani.ani_tb = nil
		self:stand()
	end
end

function sprite_body:set_dir(dir)
	self.ani:set_ani_dir(dir)
end

function sprite_body:stand()
	self:do_ani("stand")
	local ani = self.ani
	if ani.ani_tb then
		ani.on_aniend = 
			function(ended)
				--add a random timer to do again
				local tm = math.random(1,100) / 10 + 3
				g_timer:add_timer(tm,self.on_timer_stand,self)
			end
	end
	return false
end


--function sprite_body:_in_rect(x,y,room)
--	local l,t,r,b = self:get_rect(room)
--	print("sprite_body:_in_rect(x,y)",self.room,l,t,r,b,x,y)
	--return x >= self.x and x < self.x + self.w and y >= self.y and y < self.y + self.h
--	return x >= l and x < r and y >= t and y < b
--end


return sprite_body