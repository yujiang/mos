--root
	--layers
		--windows
			--childs

local root = class(cell,"root")


--layer的z值定下来就不能改变了,--加个name好定位
function root:create_root(w,h)
	cell.create_cell(self,"root",0,0,0)	

	self.w = w;
	self.h = h;
	self.drag = {}
	self.chain = {}

	local layers = {
		{"bottom",-100},
		{"map",0},
		{"normal",10},
		{"top",100},
		{"modal",1000},
	}
	for k,v in ipairs(layers) do
		local l = layer()
		l:create_layer(v[1],v[2],w,h)
		self:add_child(l)
	end

	local m = map()
	m:create_map("map",w,h,1005)
	--print(m.name)
	self:find_child("map"):add_child(m)

	self.emitter = emitter()
end

function root:get_map()
	return self:find_child("map"):find_child("map")
end

--操纵的主角。
function root:set_play(play)
	self.play = play
end

function root:get_play(play)
	return self.play 
end

--玩家居中，可能用camera的类更好。
--function root:center_play()
--	if self.play then
--		local x,y = self.play:get_pos()
--		local map = self:get_map()
--		map:center_play(x,y,self.w,self.h)
--	end
--end

function root:set_modal_window(win)
	if self.modal_window then
		--return
	end
	--print("root:set_modal_window",win.name)
	self.modal_window = win
	self:find_child("modal"):add_child(win)
	if win:is_hide() then
		win:show()
	end
end 

function root:set_popup_menu(win)
	if self.popup_menu then
		--return
	end
	--print("root:popup_menu",win.name)
	self.popup_menu = win
	self:find_child("modal"):add_child(win)
	if win:is_hide() then
		win:show()
	end
end

function root:start_drag(win,x,y)
	--print("start_drag",x,y,win.name)
	self.drag.x = win.x
	self.drag.y = win.y
	self.drag.mx = x
	self.drag.my = y
	self.drag.time = os.clock()
	if self.drag_cell then
		self.drag_cell:on_set_drag(false,win)
	end
	win:on_set_drag(true)
	self.drag_cell = win
	win:topest()
end

function root:is_draged_time()
	return os.clock() - self.drag.time >= 0.1
end

function root:on_draged(x,y)
	--加一个条件，不能drag出father的rect，否则会点不中了。
	local f = self.drag_cell.father
	assert(f)

	local ox,oy = cell.get_offset(f,self)
	if not f:in_size(x - ox,y - oy) then
		--print("not f:in_rect(x - ox,y - oy)",f.name,x,y,ox,oy,x-ox,y-oy)
		return
	end

	local win = self.drag_cell
	win.x = self.drag.x + x - self.drag.mx
	win.y = self.drag.y + y - self.drag.my
end

function root:end_drag(x,y)
	local win = self.drag_cell
	--print("end_drag",x,y,win.name)
	if self:is_draged_time() then
		self:on_draged(x,y)
	end
	self.drag_cell:on_set_drag(false)
	self.drag_cell = nil
end

function root:set_focus(win)
	local old = self.focus
	if win ~= self.focus then
		if self.focus then
			self.focus:on_get_focus(false,win)
		end
		self.focus = win
		if win then
			win:on_get_focus(true,old)
			--print("self.focus = ",win.name)
		else
			--print("self.focus = nil")
		end
	end
end

--function root:on_window_close(win)
--end

function root:set_hover(win)
	local old = self.hover
	if win ~= self.hover then
		if self.hover then
			local rt = self.hover:on_get_hover(false,win)
			--if win == nil and rt == true then --强制要求hover不变。
				--return
			--end
		end
		self.hover = win
		if win then
			win:on_get_hover(true,old)
			--print("self.hover = ",win.name)
		else
			--print("self.hover = nil")
		end

		--self.emitter:emit("on_hover",old,win)
	end
end

function root:check_var()
	if self.modal_window and self.modal_window:is_hide() then
		self:find_child("modal"):remove_child(self.modal_window)
		self.modal_window = nil
	end
	if self.popup_menu and self.popup_menu:is_hide() then
		self:find_child("modal"):remove_child(self.popup_menu)
		self.popup_menu = nil
	end
	if self.focus and self.focus:is_hide_all() then
		self:set_focus(nil)
	end
	if self.hover and self.hover:is_hide_all() then
		self:set_hover(nil)
	end
	if self.drag_cell and self.drag_cell:is_hide_all() then
		self.drag_cell:on_set_drag(false)
		self.drag_cell = nil
	end
end

function root:recv_mouse_msg(mouse_event,x,y,param)
	self:check_var()

	if self.drag_cell then
		if mouse_event == WM_MOUSEMOVE and self:is_draged_time() then
			self:on_draged(x,y)
		end
		if mouse_event == WM_LBUTTONUP then
			self:end_drag(x,y)
		end	
		return true
	end

	local win
	if self.modal_window then
		win = self.modal_window:recv_mouse_msg(mouse_event,x,y,param)
	else
		if self.popup_menu then
			win = self.popup_menu:recv_mouse_msg(mouse_event,x,y,param)
			if not win and mouse_event == WM_LBUTTONDOWN or mouse_event == WM_RBUTTONDOWN then
				self.popup_menu:hide()
				--print("self.popup_menu:hide() 1",mouse_event,win)
				win = cell.recv_mouse_msg(self,mouse_event,x,y,param)
				--print("self.popup_menu:hide() 2",mouse_event,win)
			end 
		else
			win = cell.recv_mouse_msg(self,mouse_event,x,y,param)
		end
	end

	if win and win:is_hide_all() then
		win = nil
	end

	if mouse_event == WM_MOUSEMOVE then
		self:set_hover(win)
	elseif mouse_event == WM_LBUTTONDOWN then
		self:set_focus(win)
		if win and win:can_drag() then
			self:start_drag(win,x,y)
		end	
	elseif mouse_event == WM_RBUTTONDOWN then
		if win == nil then
			ui:on_root_mouse_rd(x,y)
		end
	end

	self:check_var()
end

--chain

function root:add_chain(win)
	self:close_chain(win)
	table.insert(self.chain,win)
end

function root:close_chain(win)
	for i,w in ipairs(self.chain) do
		if win == w then
			table.remove(self.chain,i)
			return true
		end
	end
end

--顶端的
function root:top_chain()
	return self.chain[#self.chain]
end

function root:close_top_chain()
	local w = self:top_chain()
	if w then
		w:close()
	end
end

--改名防止死循环
function root:focus_recv_key_down(key,alt,ctrl,shift)
	if self.focus then
		self.focus:recv_key_down(key,alt,ctrl,shift)
		self:check_var()
		return true
	end
	self:on_key_down(key,alt,ctrl,shift)
end

function root:on_key_down(key,alt,ctrl,shift)
	if key == VK_ESCAPE then
		self:close_top_chain()
		self:check_var()
		return true
	end
end

function root:get_all_window_name()
	local names = {}
	for name,layer in pairs(self.childs) do
		if name ~= "modal" then
			layer:get_all_window_name(names)
		end
	end	
	return names
end

function root:show_all_window()
	for name,layer in pairs(self.childs) do
		if name ~= "modal" then
			layer:show_all_window()
		end
	end	
end

function root:show_window(win)
	for name,layer in pairs(self.childs) do
		if name ~= "modal" then
			layer:show_window(win)
		end
	end	
end

return root
