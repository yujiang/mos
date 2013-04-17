local ui = class("ui")

WM_LBUTTONDOWN = 1
WM_LBUTTONUP = 2
WM_RBUTTONDOWN = 3
WM_RBUTTONUP = 4
WM_MOUSEMOVE = 5
WM_MOUSEWHEEL = 6

ALIGN_DEFAULT = 0
ALIGN_CENTER = 1

local ui_files = {
	"cell",
	"root","layer","window",
	"box","image","button",
	"label","text",
	"msgbox","menu","menu_popup","mitem",
}

function ui:init_ui_libs()
	for _,file in ipairs(ui_files) do
		_G[file] = require ("ui."..file)
	end
	--auto_test()
	--test_class()
	--g_root = test_ui()
end

function ui:reload(file)
	if not table_has_value(ui_files,file) then
		return
	end
	return self:reload_ui(file)
end

function ui:reload_ui(file,path)
	path = path or "ui"
	print("reload ui",path,file)

	package.loaded[path.."."..file] = nil
	local old = _G[file]
	local new = require (path.."."..file) 
	assert(old ~= new)
	g_root:reload_class(old,new)
	_G[file] = new

	for _,file in ipairs(ui_files) do
		if _G[file]._base == old then
			self:reload(file)
		end
	end
	return true
end

--每一帧调用
function ui:on_every_frame()
	if not g_root then
		return
	end
	--g_root:center_play()

	local rd = g_root:get_render_childs()
	--print("on_every_frame rd",rd)
	--cell.print_render(rd)
	if rd then
		cdriver.render(rd)
	end

	local fps = g_timer:get_fps()
	local l = g_root:find_control("top.fps")
	if l then
		l:set_string("fps:"..fps)
	end
end

--鼠标消息
MK_CONTROL = 0x0008
MK_LBUTTON = 0x0001
MK_MBUTTON = 0x0010
MK_RBUTTON = 0x0002
MK_SHIFT = 0x0004
MK_XBUTTON1 = 0x0020
MK_XBUTTON2 = 0x0040

--某些键盘消息
VK_ESCAPE =       0x1B
VK_SPACE =        0x20
VK_PRIOR =        0x21
VK_NEXT =         0x22
VK_END =          0x23
VK_HOME =         0x24
VK_LEFT =         0x25
VK_UP =           0x26
VK_RIGHT =        0x27
VK_DOWN =         0x28
VK_SELECT =       0x29
VK_PRINT =        0x2A
VK_EXECUTE =      0x2B
VK_SNAPSHOT =     0x2C
VK_INSERT =       0x2D
VK_DELETE =       0x2E
VK_HELP =         0x2F

function bit_set(x, p)
  return x % (p + p) >= p       
end

function bit_and(a,b)
end

function ui:on_mouse_msg(mouse_event,x,y,param)
	if mouse_event == WM_LBUTTONDOWN then
		self.mouse_ld = true
	elseif mouse_event == WM_RBUTTONDOWN then
		self.mouse_rd = true
	elseif mouse_event == WM_LBUTTONUP then
		self.mouse_ld = false
	elseif mouse_event == WM_RBUTTONUP then
		self.mouse_rd = false
	end

	--如果鼠标按下drag，但是移除了屏幕up，然后再次回来
	if mouse_event == WM_MOUSEMOVE then
		if self.mouse_ld and not bit_set(param,MK_LBUTTON) then
			self:on_mouse_msg(WM_LBUTTONUP,self.mouse_x,self.mouse_y)
		elseif self.mouse_ld and not bit_set(param,MK_RBUTTON) then
			self:on_mouse_msg(WM_RBUTTONUP,self.mouse_x,self.mouse_y)
		end
	end

	self.mouse_x = x
	self.mouse_y = y
	g_root:recv_mouse_msg(mouse_event,x,y,param)
end

function ui:on_key_down(key,alt,ctrl,shift)
	return g_root:focus_recv_key_down(key,alt,ctrl,shift)
end

function ui:load_window(window)
	local win =	dofile("save/"..window..".lua")
	print ("ui:load_window",win,win.name,win.layer)
	if win then
		cell.loaded(win)
		local l = g_root:find_child(win.layer)
		l:add_window(win)
	end
end

function ui:save_window(win)
	local map = g_root:get_map()
	win.layer = win.father:get_name()
	local s = win:to_string()
	local f = io.open("save/"..win.name..".lua","w")
	f:write("local "..s)
	f:write("\nreturn "..win.name)
	f:close()
end

function ui:on_sprite_mouse_rd(win,x,y)
	--self:on_window_mouse_rd(win,x,y)
	local m = menu_popup()
	m:create_menu_texts(win.name.."_menu",self.mouse_x,self.mouse_y,0,100,20,
	{"drag","hide","destroy","save","print","printrd","ai..."},win)
	m:set_check("drag",win.drag)

	local item = m:get_item(7) --show...
	local m2 = menu()
	m2:create_child_menu(100,20,{"walk_path","walk_path loop","walk random"})
	item:set_menu(m2)
	m2.on_lclick = function (menu,btn)
		--print("ui:on_window_mouse_rd",menu.name,btn.name,btn:get_caption_string())
		local menu = btn:get_caption_string()
		--print(menu,menu == "walk_path")
		local ai = win:get_ai()
		local path = {{200,200},{500,200},{500,500},{200,500}}
		if menu == "walk_path" then
			ai:walk_path(path,100)
		elseif menu == "walk_path loop" then
			ai:walk_path(path,100,true)
		elseif menu == "walk random" then
			ai:walk_random(200,100)
		end
	end

	m.on_lclick = function (menu,btn)
		print("ui:on_window_mouse_rd",menu.name,btn.name,btn:get_caption_string())
		local menu = btn:get_caption_string()
		if menu == 'hide' then
			win:hide()
		elseif menu == 'destroy' then
			win:destroy()
		elseif menu == 'drag' then
			win.drag = not win.drag
			--m:set_check("drag",win.drag)
		elseif menu == 'save' then
			self:save_window(win)
		elseif menu == 'print' then
			win:print()
		elseif menu == 'printrd' then
			win:print_render()
		end
	end
	g_root:set_popup_menu(m)
end

function ui:on_map_mouse_rd(map,x,y)
	local m = menu_popup()
	m:create_menu_texts(map.name.."_menu",self.mouse_x,self.mouse_y,0,100,20,
	{"adds","hide","save","showall"},map)
	m:set_check("drag",map.drag)
	m.on_lclick = function (menu,btn)
		print("ui:on_window_mouse_rd",menu.name,btn.name,btn:get_caption_string())
		local menu = btn:get_caption_string()
		if menu == 'hide' then
			map:hide()
		elseif menu == 'save' then
			self:save_window(map)
		elseif menu == 'showall' then
			map:show_all_sprite()
		elseif menu == 'adds' then
			map:add_sprite(x,y,101)
		end
	end
	g_root:set_popup_menu(m)
end

function ui:on_root_mouse_rd(x,y)
	local m = menu_popup()
	m:create_menu_texts("root_menu",self.mouse_x,self.mouse_y,0,100,20,{"load","showall","show..."},win)
		
	local item = m:get_item(3) --show...
	local m2 = menu()
	m2:create_child_menu(100,20,g_root:get_all_window_name())
	item:set_menu(m2)

	m.on_lclick = function (menu,btn)
		--print("ui:on_window_mouse_rd",menu.name,btn.name,btn:get_caption_string())
		local menu = btn:get_caption_string()
		if menu == 'showall' then
			g_root:show_all_window()
		elseif menu == "load" then
			print("please input: l [win name]")
		end
	end
	m2.on_lclick = function (menu,btn)
		--print("ui:on_window_mouse_rd",menu.name,btn.name,btn:get_caption_string())
		local menu = btn:get_caption_string()
		g_root:show_window(menu)
	end

	g_root:set_popup_menu(m)		
end

--统一处理window的某些事件
function ui:on_window_mouse_rd(win,x,y)
	--编辑器的方法。
	local m = menu_popup()
	m:create_menu_texts(win.name.."_menu",self.mouse_x,self.mouse_y,0,100,20,
	{"drag","hide","destroy","save","print","printrd"},win)
	m:set_check("drag",win.drag)
	m.on_lclick = function (menu,btn)
		print("ui:on_window_mouse_rd",menu.name,btn.name,btn:get_caption_string())
		local menu = btn:get_caption_string()
		if menu == 'hide' then
			win:hide()
		elseif menu == 'destroy' then
			win:destroy()
		elseif menu == 'drag' then
			win.drag = not win.drag
			--m:set_check("drag",win.drag)
		elseif menu == 'save' then
			self:save_window(win)
		elseif menu == 'print' then
			win:print()
		elseif menu == 'printrd' then
			win:print_render()
		end
	end
	g_root:set_popup_menu(m)
end

--font
function ui:get_font_height(fontid)
	if not regist_fonts then
		return
	end
	return regist_fonts[fontid]
end

function ui:regist_font(fontid,ttf,width,height,xspace,yspace)
	if not regist_fonts then
		regist_fonts = {}
	end
	regist_fonts[fontid] = height + yspace
	cdriver.regist_font(fontid,ttf,width,height,xspace,yspace)
end

--test
function test_ui(w,h)
	cdriver.regist_image("play",1,"play-button.png")
	cdriver.regist_image("play",2,"play-button.png")
	cdriver.regist_image("play",3,"play-button-down.png")
	cdriver.regist_image("dog",1,"dog.png",0,0,105,95)
	cdriver.regist_image("dog",2,"dog.png",105,0,210,95)
	cdriver.regist_image("dog",3,"dog.png",105,0,210,95)

	local r = root()
	r:create_root(w,h)

	local l1 = r:find_child("top")

	local win3 = window()
	win3:create_window("win3",200,200,10,200,200)
	l1:add_window(win3)
	win3:set_bg("land.png")
	win3:get_bg().alpha = 200
	
	local img = image()
	img:create_image("ani_dog",40,-40,0,"dog",1)
	win3:add_child(img)
	img:set_ani(0.5,1,3,true)

	local mv = win3:get_move()
	mv:move_to(400,400,100)

	local win4 = window()
	win4:create_window("win4",10,10,20,200,100)
	win4:set_box(0x008080ff,180)

	l1:add_window(win4)
	local label1 = label()
	local s1 = "1.fps:100√\n2.hello world!HAPPY.\n3.你好，世界！\n4.long long ago."
	for i=5,20 do 
		s1 = s1 .."\n".. i.. ".test s"
	end
	label1:create_label("lable1",0,0,0,200,100,s1)
	label1._disable = false
	label1:get_text():set_scroll(4)
	--label1:create_label("lable1",0,0,0,100,60,"好a好b若c您d为e了！")
	--label1:set_color(0x00ff00)
	label1:get_text():set_color(0x00ff8080)
	win4:add_child(label1)

	local label1 = label()
	label1:create_label("fps",0,0,0,100,20,"fps:100")
	label1._disable = false
	label1:get_text():set_color(-1)
	l1:add_child(label1)

	local l2 = r:find_child("normal")

	local win2 = window()
	--win2.is_chain = true
	win2:create_window("win2",200,200,10,200,200)
	l2:add_window(win2)
	win2:set_bg("menu2.png")

	local win = window()
	--win.is_chain = true
	win:create_window("win1",100,100,0,200,200)
	l2:add_window(win)
	win:set_bg("menu2.png")
	--win:set_bg("farm.jpg")
	--win:get_bg().alpha = 200
	win.alpha = 200
	--win:set_bg("dog.png")

	local btn = button()
	btn:create_button("btn1",20,20,0)
	btn:set_bg("play")
	btn:set_caption("游戏",0,50,50)
	local text = btn:get_caption_text()
	text.color = 0x000000ff
	win:add_child(btn)
	assert(btn == r:find_control("normal.win1.btn1"))

	local btn2 = button()
	btn2:create_button("dog",200,100,0)
	btn2:set_bg("dog")
	btn2:set_caption("狐狸")
	btn2:get_caption():set_align(ALIGN_CENTER)
	local text = btn2:get_caption_text()
	text.color = 0x0000ff00
	win:add_child(btn2)

	local m = r:get_map()
	m:set_bg("farm.jpg")

	g_ani_data:regist_ani_data(101,"stand","dog",0,1,1,false,50,80)
	g_ani_data:regist_ani_data(101,"walk","dog",0.25,1,3,true,50,80)

	local sp = sprite()
	sp:create_sprite(nil,100,100,0,101)
	m:add_child(sp)
	sp:stop()
	sp:set_name("精灵",0x00ff00)

	r:set_play(sp)

	--local sp = sprite()
	--sp:create_sprite("dog2",100,100,0)
	--m:add_child(sp)

	return r
end

return ui	