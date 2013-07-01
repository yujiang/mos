--µØÍ¼

local map = class(window,"map")

function map:create_map(name,w,h,id)
	assert(id)
	window.create_window(self,name,0,0,0,w,h)
	self.drag = false

	local jpg = string.format("map/%04d.jpg",id)
	self:set_bg(jpg)
end

function map:mouse_2_map(x,y)
	return (x-self.x)/self.room,(y-self.y)/self.room
end

function map:add_sprite(x,y,ani)	
	--x,y is mouse pos
	local sp = sprite()
	local x2,y2 = self:mouse_2_map(x,y)
	sp:create_sprite(nil,x2,y2,0,101)
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
	self.x = w/2 - x*self.room
	self.y = h/2 - y*self.room
	--print("center_play",self.x,self.y,x,y,self.room)
--	if self.x > 0 then
--		self.x = 0
--	end
--	if self.x + self.w < w then
--		self.x = w - self.w
--	end
--	if self.y > 0 then
--		self.y = 0
--	end
--	if self.y + self.h < h then
--		self.y = h - self.h
--	end
	--print("map:center_play()",self.x,self.y,w,h)
end


function map:on_mouse_msg(mouse_event,x,y,param)
	--if mouse_event == WM_RBUTTONDOWN then
		--print("window:on_mouse_msg WM_RBUTTONDOWN",self.name,x,y,self:in_rect(x,y))
	--end
	if mouse_event ~= WM_MOUSEMOVE then
		--print("map:on_mouse_msg",mouse_event,x,y)
	end

	if mouse_event == WM_LBUTTONDOWN then
		if g_root.play then
			local x2,y2 = self:mouse_2_map(x,y)
			g_root.play:walk_to(x2,y2,150)
		end
		--self:topest()
	elseif mouse_event == WM_RBUTTONDOWN then
		ui:on_map_mouse_rd(self,x,y)
	elseif mouse_event == WM_MOUSEWHEEL then
		--print("map:on_mouse_msg","WM_MOUSEWHEEL")
		self:on_mouse_scroll_room(param,0.5,2)
	end

	--cell.on_mouse_msg(self,mouse_event,x,y)
	return self
end

return map