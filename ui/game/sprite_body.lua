
local sprite_body = class(image,"sprite_body")

function sprite_body:create_body(name,x,y,z,ani_id,use_zgp)
	--self:set_image(image_file,frame)
	print("sprite_body:create_body",name,x,y,z)
	cell.create_cell(self,name,x,y,z,0,0)
	self.ani_id = ani_id
	self.ani_id2 = ani_id2
	self.use_zgp = use_zgp
	self._disable = true
end

function sprite_body:change_frame(frame)
	image.change_frame(self,frame)
	if self:get_weapon() then
		self:get_weapon():change_frame(frame)
	end
end

function string_rfind(s,pat)
	local m,n
	local len = 0
	while(true) do
		i,j = string.find(s,pat)
		if (j == nil) then
			break
		end
		m,n = i,j
		s = string.sub(s,j+1)
		len = len + j
		--print(s,len)
	end
	if (len == 0) then
		return
	end
	return m+len-n,n+len-n
end

function sprite_body:set_image(image_file,frame)
	local w = self:get_weapon()
	if w then
		--从image_file来才对；self.ani_name可能是个别名并不等于文件名。
		local i,j = string_rfind(image_file,"/")
		print(image_file,i,j)
		local snum = string.format("%02d/",w.weapon_id)
		local s = string.sub(image_file,1,i) .. snum .. string.sub(image_file,i+1)
		print(s)
		--local s = string.format("char/%04d/%02d/%s.zgp",self.ani_id,w.weapon_id,self.ani_name)
		w:set_image(s,frame)
	end
	print("image.set_image",image_file,frame)
	image.set_image(self,image_file,frame)
end

function sprite_body:get_weapon()
	return self:find_child("weapon")
end

function sprite_body:set_weapon(weapon_id)
	local weapon = image()
	weapon:create_cell("weapon",0,0,1,0,0)
	self:add_child(weapon)
	weapon.weapon_id = weapon_id
	--assert(self:get_weapon())	
end

function sprite_body:do_ani(ani_name)
	self.ani_name = ani_name
	local tb = g_ani_data:find_ani_data(self.ani_id,ani_name,self.use_zgp)	
	if not tb then
		print("error! sprite:do_ani ",self.ani_id,ani_name)
		assert(tb)
		return
	end
	
	self:set_ani_tb(tb)
end

function sprite_body:on_timer_stand()
	--print("sprite:on_timer_stand()")
	local ani = self.ani
	if ani.ani_tb.name == "stand" then
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
	ani.on_aniend = 
		function(ended)
			--add a random timer to do again
			local tm = math.random(1,100) / 10 + 3
			g_timer:add_timer(tm,self.on_timer_stand,self)
		end
	return false
end



return sprite_body