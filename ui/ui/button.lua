--require "image"

BUTTON_NORMAL = 1
BUTTON_HOVER = 2
BUTTON_PRESS = 3

--游戏的button都是image，没有像windows gui那样用通用的

local button = class(cell,"button")

--files放三态按钮，normal，hover，push
function button:create_button(name,x,y,z,w,h)
	--image.create_image(self,name,x,y,z,file,status)
	cell.create_cell(self,name,x,y,z,w,h)
	self.status = BUTTON_NORMAL
	--self._disable = false;
end

function button:set_bg(image)
	cell.set_bg(self,image,self.status)
end

function button:change_status(status)
	self.status = status
	if self:get_bg() then
		self:get_bg().frame = status
	end

	if self:get_box() and self.status_box_colors then
		self:get_box().color = self.status_box_colors[status]
	end

	--print("button:change_status",status)
end

function button:get_status()
	return self.status
end

function button:on_lclick()
end

--这个方便调试。
function button:_on_lclick()
	print("button:_on_lclick()",self.name)
	self:on_lclick()
end

function button:on_get_focus(focus)
	cell.on_get_focus(self,focus)
	if not focus and self.status == BUTTON_PRESS then
		self:change_status(BUTTON_NORMAL)
	end
end

function button:on_get_hover(mouse)
	self:change_status(mouse and BUTTON_HOVER or BUTTON_NORMAL)
end

function button:on_mouse_msg(mouse_event,x,y,param)
	if mouse_event == WM_LBUTTONDOWN then
		self:change_status(BUTTON_PRESS)
	elseif mouse_event == WM_MOUSEMOVE then
		--self:change_status(BUTTON_HOVER)
	elseif mouse_event == WM_LBUTTONUP then
		--print("button:on_click",self.name,x,y)--,self.x,self.y,self.w,self.h)
		if self.status == BUTTON_PRESS then
			self:_on_lclick()
			self:change_status(BUTTON_HOVER)
		end
	end
	return self
end

function button:on_key_down(key,alt,ctrl,shift)
	--print("button:on_key_down",key)
	if is_char(key," ") then
		self:on_lclick()
		return true
	end
end

return button