--cell base function

local cell = class("cell")

--故意不用同一个init
function cell:create_cell(name,x,y,z,w,h)
	assert(self.class_name)
	self.name = name
	self.x = x
	self.y = y
	self.z = z
	self.w = w
	self.h = h
	self.childs = {} 
end

--create and destroy
function cell:destroy()
	if self.father then
		self.father:remove_child(self)	
		self.father = nil
	end
	self._destroy = true
	for _,child in pairs(self.childs) do
		child:on_destroyed()
	end
	if self.move then
		self.move:stop_move()
	end
	self:on_destroyed()
end

function cell:on_destroyed()
end

function cell:is_valid()
	return not self._destroy
end

--name,pos,size,rect
function cell:get_name()
	return self.name
end

function cell:get_pos()
	return self.x,self.y
end

function cell:get_pos_all()	
	local x,y = self:get_pos()
	local ox,oy = self:get_offset_all()
	return x+ox,y+oy
end

function cell:get_offset_all()	
	local x = 0
	local y = 0
	local f = self.father
	while(f) do
		local x2,y2 = f:get_pos()
		x = x + x2
		y = y + y2
		f = f.father
	end
	return x,y
end

function cell:set_pos(x,y)
	self.x,self.y = x,y
end

function cell:offset(x,y)
	self.x,self.y = self.x + x,self.y + y
end

function cell:get_size()
	return self.w,self.h
end

function cell:set_size(w,h)
	self.w,self.h = w,h
end

function cell:in_rect_all(x,y)
	local ox,oy = self:get_offset_all()
	return self:in_rect(x-ox, y-oy)
end

function cell:in_x(x)
	return x >= self.x and x < self.x + self.w 
end

function cell:in_y(y)
	return y >= self.y and y < self.y + self.h
end

function cell:in_size(x,y)
	return x >= 0 and x < self.w and y >= 0 and y < self.h
end

function cell:get_rect()
	return self.x,self.y,self.x + self.w,self.y + self.h
end

function cell:_in_rect(x,y)
	local l,t,r,b = self:get_rect()
	--return x >= self.x and x < self.x + self.w and y >= self.y and y < self.y + self.h
	return x >= l and x < r and y >= t and y < b
end

--只有image重载这两个函数，所以没有写成in_rect_override的形式？
--实际上，下拉菜单menu也重载了，但是menu属于内部使用的。
function cell:in_rect(x,y)
	local bg = self:get_bg() 
	if bg then 
		return bg:in_rect(x-self.x,y-self.y)
	end
	return self:_in_rect(x,y)
end

function cell:in_rect_pixel(x,y)
	local bg = self:get_bg() 
	if bg then 
		return bg:in_rect_pixel(x-self.x,y-self.y)
	end
	return self:_in_rect(x,y)
end

---------------------------------------------------------
--bg and box and caption
function cell:set_bg(image_file,frame)
	local bg = image()
	--z随便给个负值，放在最下面
	bg:create_image("bg",0,0,-1000,image_file,frame or 0)
	self:add_child(bg)
	self.w = bg.w
	self.h = bg.h
end

function cell:get_bg()
	return self:find_child("bg")
end

function cell:set_box(color,alpha)
	local b = box()
	--z随便给个负值，放在最下面
	b:create_box("box",0,0,-2000,self.w,self.h,color,alpha)
	self:add_child(b)
end

function cell:get_box()
	return self:find_child("box")
end

function cell:set_caption(text,font,offx,offy)
	offx = offx or 0
	offy = offy or 0
	local lb = label()
	assert(self.w >= 0 and self.h >= 0)
	lb:create_label("caption",offx,offy,1000,self.w-2*offx,self.h-2*offy,text,font,nil,ALIGN_CENTER)
	self:add_child(lb)
end

function cell:get_caption()
	return self:find_child("caption")
end

function cell:get_caption_text()
	return self:get_caption():get_text()
end

function cell:get_caption_string()
	return self:get_caption():get_string()
end


--放缩先不考虑
function cell:get_room()
	return self.room 
end

--color and alpha, 可惜没有bitop，否则可以二合为一
function cell:get_alpha()
	return self.alpha
end

function cell:set_alpha(a)
	self.alpha = a
end

function cell:get_color()
	return self.color
end

function cell:set_color(c)
	self.color = c
end

--hide and show
function cell:hide()
	self._hide = true
end

function cell:is_hide()
	return self._hide
end

function cell:is_hide_all()
	local c = self
	while(c) do
		if c:is_hide() then
			return true
		end
		c = c.father
	end
	return false
end

function cell:show()
	self._hide = false
end

function cell:is_show()
	return not self:is_hide()
end

--enable and disable
function cell:disable()
	self._disable = true
end

function cell:is_disable()
	return self._disable 
end

function cell:enable()
	self._disable = false
end

function cell:is_enable()
	return not self:is_disable()
end

function cell:is_disable_all()
	local c = self
	while(c) do
		if c:is_disable() then
			return true
		end
		c = c.father
	end
	return false
end

--是否接受msg
function cell:is_msg()
	return self:is_show() and self:is_enable()
end

--是否可以拖动
function cell:can_drag()
	return self.drag
end

--z
function cell:get_z()
	return self.z
end

--window以z排序，按z值的排序绘制，z越大越在前面挡住其他的。
function cell:max_z()
	--print("cell:max_z()",self.name)
	local maxz = 0
	for name,w in pairs(self.childs) do
		--print("cell:max_z()",name,w.z)
		if w.z > maxz then
			maxz = w.z
		end
	end	
	return maxz
end

function cell:topest_z()
	return self:max_z()
end

function cell:topest()
	assert(self.father)
	self.z = self.father:topest_z() + 1
	--print("cell:topest()",self.name,self.z)
end

-------------------------------------------------------
--child and father
--使用name而不用id，因为name阅读性更强，注意name不能改变并且在该层次唯一。
function cell:add_child(child)
	self.childs = self.childs or {}
	local old = self.childs[child.name]
	if old then
		old:destroy()
	end
	self.childs[child.name] = child
	child.father = self
end

function cell:get_childs_num()
	return table_kv_size(self.childs)
end

--最好没有remove，直接hide即可，无非是点内存
function cell:remove_child(child)
	self.childs[child.name] = nil
end

function cell:find_child(name)
	return self.childs[name]
end

function cell.get_offset(from,to)
	local tb = cell.get_father_chain(from,to)
	if not tb then
		return
	end
	local x = 0
	local y = 0
	for _,c in ipairs(tb) do
		x = x + c.x
		y = y + c.y
	end
	return x,y
end

function cell.get_father_chain(from,to)
	local tb = {}
	while(from) do
		table.insert(tb,from)
		from = from.father
		if from == to then
			return tb
		end
	end
end

function cell.get_child_chain(from,to)
	local tb = cell.get_father_chain(from,to)
	if not tb then
		return
	end
	local t = {}
	for i = #tb,1,-1 do
		table.insert(t,tb[i])
	end
	return t
end

--like find top.loginwindow.loginctrl.bg
function cell:find_control(full_name)
	local child_name,next_name = split2(full_name,"%.")
	child_name = child_name or full_name
	local child = self:find_child(child_name)
	if child then
		if next_name then
			return child:find_control(next_name)
		else
			return child
		end
	else
		--print("error! not find "..child_name .. " of "..self.name)
		--table_print_kv(self.childs)
	end
end

function cell:is_father_all(f)
	assert(f)
	local c = self.father
	while c do
		if c == f then
			return true
		end
		c = c.father
	end
	return false
end

function cell:is_child_all(c)
	for _,child in pairs(self.childs) do
		if child == c then
			return true
		elseif child:is_child_all(c) then
			return true
		end
	end
	return false
end

----------------------------------------------- draw
--为了绘制剔除了hide的window
local function get_sortshow_tb(t,func,great)
	--print("get_sortshow_tb",table_kv_size(t))
	local tb = {}
	for name,l in pairs(t) do
		if func(l) then
			table.insert(tb,l)
		end
	end
	if great then
		table.sort(tb,function(l1,l2) return l1:get_z() > l2:get_z() end)
	else
		table.sort(tb,function(l1,l2) return l1:get_z() < l2:get_z() end)
	end
	return tb
end

--layer最多5,6层，
--每个layer最多5,6个window，
--所以sort一下也无所谓。
function cell:get_sortshow_childs()
	return get_sortshow_tb(self.childs,self.is_show)
end

function cell:get_sortmsg_childs()
	--great 排序反过来
	return get_sortshow_tb(self.childs,self.is_msg,true)
end

--重载这个得到渲染对象。
function cell:get_render_override()
end

--render
--虚函数 get_render_override,不用在lua层确定offset，在c++层确定即可。
function cell:get_render_childs()
	if self:get_childs_num() > 0 then
		local tb = {name = self.name,x = self.x,y = self.y,z = self.z,color = self.color,alpha = self.alpha}
		local shows = self:get_sortshow_childs()
		for _,child in pairs(shows) do
			local t = child:get_render_childs()
			if t then
				table.insert(tb,t)
			end
		end	
		if #tb == 0 then
			return
		end
		return tb
	else
		return self:get_render_override() --or cell
	end
end

--打印render,相当于static的函数。
function cell:print_render(tb)
	if not tb then
		tb = self:get_render_childs()
	end
	local ignores = {father = true,childs = true}
	table_print(tb,nil,nil,ignores)
end

--打印cell
--move和ai自己重建，而不存储了。
local print_ignores = {father = true,move = true,ai = true}
function cell:print()
	table_print(self,nil,nil,print_ignores)
end

function cell:to_string()
	return table.show(self,self.name,nil,print_ignores)
end

--window的console不支持utf8的输出，所以要改下。

--关于消息
function cell:on_mouse_msg(mouse_event,x,y,param)
end

function cell:on_key_down(key,alt,ctrl,shift)
end

function cell:recv_key_down(key,alt,ctrl,shift)
	if self:on_key_down(key,alt,ctrl,shift) then
		return true
	end
	if self.father then
		return self.father:recv_key_down(key,alt,ctrl,shift)
	end
end

function cell:on_child_mouse_msg(mouse_event,x,y,param,child)
end

--也只有menu重载这个函数
--menu是内部使用。
function cell:recv_mouse_msg(mouse_event,x,y,param)
	if mouse_event == WM_MOUSEWHEEL then
		--print("recv_mouse_msg WM_MOUSEWHEEL",self.name,x,y,param,self:in_rect(x,y))
	end
	if not self:in_rect(x,y) then
		return
	end

	local x2 = x - self.x
	local y2 = y - self.y
	local msgs = self:get_sortmsg_childs()
	for _,child in pairs(msgs) do
		local rt = child:recv_mouse_msg(mouse_event,x2,y2,param)
		if rt then
			self:on_child_mouse_msg(mouse_event,x,y,param,rt)
			return rt
		end
	end	

	if mouse_event == WM_MOUSEWHEEL then
		--print("recv_mouse_msg WM_MOUSEWHEEL",self.name,self:in_rect_pixel(x,y),self:in_rect(x,y))
	end
	if self:in_rect_pixel(x,y) then
		return self:on_mouse_msg(mouse_event,x,y,param)	
	end
end

--一些与root的对应
function cell:on_get_focus(is_focus,focus) 
	self.is_focus = is_focus
end

function cell:on_get_hover(is_hover,hover)
	self.is_hover = is_hover
end

function cell:on_set_drag(is_drag,drag)
	self.is_drag = is_drag
end

--没有意义
--function cell:on_set_modal(is_modal,modal)
--	self.is_modal = is_modal
--end

--function cell:on_set_popup(is_popup,popup)
--	self.is_popup = is_popup
--end

------------------------------------------------------
function cell.assign_class(c,class_name)
	local t = {
		box = box,
		button = button,
		cell = cell,
		image = image,
		label = label,
		layer = layer,
		menu = menu,
		menu_popup = menu_popup,
		mitem = mitem,
		msgbox = msgbox,
		root = root,
		text = text,
		window = window,
		sprite = sprite,
		map = map,
	}
	if t[class_name] then
		setmetatable(c,t[class_name])
	else 
		print("error! assign_class "..class_name.." not find")
	end
end

function cell:on_loaded_from_table()
end

function cell:load_from_table()
	for k,v in pairs(self.childs) do
		--because father is ignored. so re assign to self.
		v.father = self
		cell.assign_class(v,v.class_name)
		v:load_from_table()
	end
	v:on_loaded_from_table()
end

function cell.loaded(win)
	cell.assign_class(win,win.class_name)
	win:load_from_table()
end
------------------------------------------------------
--reload
function cell:on_reload_class(old,new)
end

function cell:reload_class(old,new)
	--更新最顶级的类没有问题
	--注意对于基类，更新cell无法更新
	if getmetatable(self) == old then
		setmetatable(self,new)
	end
	for _,child in pairs(self.childs) do
		child:reload_class(old,new)
	end

	if getmetatable(self.move) == old then
		setmetatable(self.move,new)
	end

	self:on_reload_class(old,new)
end

------------------------------------------------------
function cell:get_move()
	if not self.move then
		self.move = move()
		self.move:create_move(self)
		--可以用在屏幕的动态移动的特效中。
	end
	return self.move
end

return cell
 