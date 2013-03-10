--lua自己也可以测试逻辑，不依赖c++，通过monk来做
--项目初期使用，现在已经废弃

function init_monk_libs()
	local clib_files = {
		"cdriver",
	}
	for _,file in ipairs(clib_files) do
		_G[file] = require ("clib."..file)
	end
end

function monk()
	require ("main")
	init_main()
	init_monk_libs()
	g_root = test_ui()
	ui:on_every_frame()
end

function auto_test()
	for k,v in pairs(_G) do
		if type(k) == "string" and type(v) == "function" and string.sub(k,1,5) == "test_" then
			print(k,"start")
			v()
			print(k,"end\n")
		end
	end
end


monk()