--c++ 的初始化和消息入口
require "test"

function init_libs(dir,files)
	for _,file in ipairs(files) do
		--print("require",dir.."."..file)
		if dir == "ex" then
			require(dir.."."..file)
		else
			_G[file] = require(dir.."."..file)
		end
	end
end

local ex_files = {
	"sort_ex",
	"string_ex",
	"table_ex",
	"class",
}

local core_files = {
	"emitter",
	"timer",
	"timer_list",
	"timer_map",
	"timer_vector",
	"timer_everyframe",
}

local db_files = {
	"ani_data",
}

local game_files = {
	"ani",
	"sprite", "sprite_body",
	"move","ai",
	"map","image_map",
	"camera",
}

function init_main()
	init_libs("ex",ex_files)
	init_libs("core",core_files)

	ui = require("ui.ui")
	ui:init_ui_libs()

	init_libs("game",game_files)
	init_libs("db",db_files)

	g_timer = timer()
	g_timer:create_timer()

	g_ani_data = ani_data()
	g_ani_data:create_ani_data()
end

function safe_on_every_frame()
	g_timer:on_every_frame()
	ui:on_every_frame()
	--may be game.on_every_frame()
end

function safe_on_mouse_event(mouse_event,x,y,param)
	assert(mouse_event)
	if mouse_event ~= WM_MOUSEMOVE then
		--print("on_mouse_event",mouse_event,x,y)
	end
	local rt = ui:on_mouse_msg(mouse_event,x,y,param)
	--may be game.on_mouse_msg(mouse_event,x,y)
end

function is_keyboard(key,s)
	return s:byte(1) == key
end

function safe_on_key_down(key,alt,ctrl,shift)
	local rt = ui:on_key_down(key,alt,ctrl,shift)
	if rt then
		return
	end
	--print("safe_on_key_down",key,alt,ctrl,shift)
	if is_keyboard(key,'M') and not ctrl then
		t_msgbox()
	end
	if is_keyboard(key,'M') and ctrl then
		dofile_lua("main")
	end
end

function reload(file)
    --print("reload",file)
	local dir_files = {
		ui = {"ui"},
		ex = ex_files,
		core = core_files,
		db = db_files,
	}

	for k,v in pairs(dir_files) do
		if table_has_value(v,file) then
			print("reload ",k,file)
			package.loaded[k.."."..file] = nil
			if k == "ex" then
				require(k.."."..file)			
			else
				_G[file] = require(k.."."..file)
			end
			return true
		end
	end

	if table_has_value(game_files,file) then
		ui:reload_ui(file,"game")
	else
		if ui:reload(file) then
		else
			print("reload not find ",file)
		end
	end
end

function dofile_lua(file)		
	local s = string.ends(file,".lua") and file or file..".lua" 
	print("dofile_lua",s)
	dofile(s)
end

function show(win,hide)
	for _,l in pairs(g_root.childs) do
		local win = l:find_child(win)
		if win then
			return hide and win:hide() or win:show()
		end
	end
end

function safe_on_init()
	init_main()
	local w = 800
	local h = 600
	local render = "gdi"
	local render = "opengl"
	cdriver.create_window("main","test",100,100,w,h,0,render);

	ui:regist_font(0,"simhei.ttf",16,16,1,2);

	g_root = root()
	g_root:create_root(w,h)
	
	g_camera = camera()
	g_camera:create_camera(g_root:get_map())
	g_camera:run_normal()

	test_fps()
	--test_png8_shader()
	--test_map()
	regist_dog()
	test_sprite(false)
	
	--test_win1()
	--test_win4()
	--test_notice()

	print("input ? or test() get example.")
end

--init()

--重用函数
function safe_on_input(s) 
	--print("on_input",s)
	--print(string.split(s))
	local t = split(s, " ")
	function find_des(name)
		if name == "r" then
			return g_root
		end
		if name == "p" then
			return g_root:get_play()
		end
	end
	if t[1] == '?' then
		print("r reload ")
		print("l load window ")	
		print("s show window ")
		print("df dofile ")
		print("p r(p) g_root(play) print ")
		print("pr r(p) g_root(play) print_render")
		print("prp g_root.play print_render")
		print("dump cdriver.dump_resource ")
		print("test() get example.")
	elseif t[1] == 's' then
		show(t[2],t[3])
	elseif t[1] == 'pr' then
		local des = find_des(t[2])
		if des then
			des:print_render()
		end
	elseif t[1] == 'p' then
		local des = find_des(t[2])
		if des then
			des:print()
		end
	elseif t[1] == 'r' then
		reload(t[2])
	elseif t[1] == 'l' then
		ui:load_window(t[2])
	elseif t[1] == 'df' then
		dofile_lua(t[2])
	--elseif t[1] == 'ds' then
		--assert(loadstring(t[2]))()
	elseif t[1] == 'dump' then
		cdriver.dump_resource(t[2])
	else
		--print("error! can not parse the input",s)
		loadstring(s)()
	end
end


-- call from c++ part
function safe_call(f)
	local rt,msg = xpcall(
		f,function(err) return debug.traceback(err) end)
	if not rt then
		print(msg)
	end
end

function on_init()
	safe_call(safe_on_init)
end

function on_input(s) 
	safe_call(function() safe_on_input(s) end)
end

function on_every_frame()
	safe_call(safe_on_every_frame)
end

function on_mouse_event(mouse_event,x,y,param)
	safe_call(function() safe_on_mouse_event(mouse_event,x,y,param) end)
end

function on_mouse_wheel(delta,x,y)
	--print("on_mouse_wheel",delta,x,y)
	on_mouse_event(WM_MOUSEWHEEL,x,y,delta)
end

function on_key_down(key,alt,ctrl,shift)
	safe_call(function() safe_on_key_down(key,alt,ctrl,shift) end)
end