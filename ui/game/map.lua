--µØÍ¼

local map = class(window,"map")

function map:create_map(name,w,h)
	window.create_window(self,name,0,0,0,w,h)
	self.drag = false
end

function map:add_sprite(x,y,ani)
	local sp = sprite()
	sp:create_sprite(nil,x-self.x,y-self.y,0,101)
	sp:stop()		
	sp:set_name(nil,math.random(1,0xffffff))	
	self:add_child(sp)
end

function map:show_all_sprite()
	for k,s in pairs(self.childs) do
		s:show()
	end
end

function map:on_child_mouse_msg(mouse_event,x,y,param,child)
	if mouse_event == WM_LBUTTONDOWN  then
	elseif mouse_event == WM_RBUTTONDOWN then
	end
end

function map:center_play(x,y,w,h)
	--print("map:center_play()",x,y,w,h)
	self.x = w/2 - x
	self.y = h/2 - y
	if self.x > 0 then
		self.x = 0
	end
	if self.x + self.w < w then
		self.x = w - self.w
	end
	if self.y > 0 then
		self.y = 0
	end
	if self.y + self.h < h then
		self.y = h - self.h
	end
	--print("map:center_play()",self.x,self.y)
end


function map:on_mouse_msg(mouse_event,x,y,param)
	--if mouse_event == WM_RBUTTONDOWN then
		--print("window:on_mouse_msg WM_RBUTTONDOWN",self.name,x,y,self:in_rect(x,y))
	--end
	if mouse_event ~= WM_MOUSEMOVE then
		--print("window:on_mouse_msg",mouse_event,x,y)
	end
	if mouse_event == WM_LBUTTONDOWN then
		if g_root.play then
			g_root.play:walk_to(x-self.x,y-self.y,100)
		end
		--self:topest()
	elseif mouse_event == WM_RBUTTONDOWN then
		ui:on_map_mouse_rd(self,x,y)
	end
	--cell.on_mouse_msg(self,mouse_event,x,y)
	return self
end

return map