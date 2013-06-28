--for menu item
local mitem = class(button,"mitem")

function mitem:create_mitem(index,w,htext,text)
	button.create_button(self,tostring(index),0,(index-1)*htext,0,w,htext)
	--self:set_box(-1,200)
	self:set_caption(text)
	self:get_caption_text():set_color(0)
	self:set_box(-1,200)
	self.htext = htext
end

function mitem:set_check(check)
	--print("mitem:set_check",check,self.name)
	local c = self:find_child("check") 
	if not c then
		c = label()
		local h = self.htext
		c:create_label("check",self.w - h,0,1001,h,h,"",0,0)
		c:get_text():set_color(0)
		self:add_child(c)
	end

	c:set_string(check and "√" or "")
	--self:print_render()
end

function mitem:disable()
	cell.disable(self)
	self:get_caption_text():set_color(0x00808080)
end

function mitem:enable()
	cell.enable(self)
	self:get_caption_text():set_color(0)
end

function mitem:set_menu(menu)
	menu.name = "menu"
	menu.x = self.w
	menu.y = 0
	menu:hide()
	self:add_child(menu)
	if menu.item_num == 0 then
		self:disable()
	end
end

function mitem:get_menu()
	return self:find_child('menu')
end

function mitem:on_get_hover(mouse,hover)
	button.on_get_hover(self,mouse,hover)
	local menu = self:get_menu()
	if not menu then
		return 
	end
	
	if mouse then
		menu:show()
		--rect 也需要改变。
	else
		if hover and (hover:is_father_all(self) or self.father == hover)then
		else
			--print("mitem.on_get_hover false and hide ",hover and hover.name)
			menu:hide()
		end
	end
end

function mitem:on_mouse_msg(mouse_event,x,y,param)
	if not self:get_menu() then
		return button.on_mouse_msg(self,mouse_event,x,y,param)
	end
	return self
end

function mitem:in_rect(x,y,room)
	if cell.in_rect(self,x,y,room) then
		return true
	end
	local menu = self:get_menu()
	if menu and menu:is_show() then
		local x2 = x - self.x
		local y2 = y - self.y
		local rt = menu:in_rect(x2,y2,room)
		if rt then 
			return true
		end
		--print("mitem:in_rect",self:in_x(x) , menu:in_y(y2))
		--if self:in_x(x) and menu:in_y(y2) then
			--return true
		--end
	end
end


--there will be function create_menu_images()...
return mitem