--c++ 的初始化和消息入口

function test_timer1()
	g_timer:add_timer(0.9,function() print("test_timer_0.9") end)
	g_timer:add_timer(1.1,function() print("test_timer_1.1") end)
	g_timer:add_timer(1,function() print("test_timer_1") end)

	local i9 = 1
	local i10 = 1
	local i11 = 1
	g_timer:add_timer(0.9,function() print("test_timer_0.9",i9) i9 = i9+1 return i9<22 end)
	g_timer:add_timer(1.1,function() print("test_timer_1.1",i11) i11 = i11+1 return i11<20 end)
	g_timer:add_timer(1,function() print("test_timer_1",i10) i10 = i10+1 return i10<21 end)
end

function test_timer3()
	local start = os.time()
	for i = 1,100 do
		g_timer:add_timer_type(
			1.0,
			function() 
				local t = math.floor(os.time() - start) 
				print("test_timer3",i,t) 
				return t < i / 10
			end
			,nil,3)
	end
end

function test_timer2()
	local start = os.time()
	for i = 1,10 do
		g_timer:add_timer_type(
			1.0,
			function() 
				local t = math.floor(os.time() - start) 
				print("test_timer2",i,t) 
				return t < i 
			end
			,nil,2)
	end
end

function test_emitter()
	local e = emitter()
	e:on("test",function (w) print("hello",w) end)
	local rt = e:on("test",function (w) print("hello2",w) return true end)
	e:emit("test","world1")
	e:emit("test","world2")
	rt.invalid = true
	e:emit("test","world3")
end

function test_msgbox()
	local mb = msgbox()
	mb:create_msgbox("msgbox",100,100,100,100)
	mb:set_bg("dog.png")
	local btn = button()
	btn:create_button("msgbox_btn1",60,20,0)
	btn:set_bg("menu1.png")
	mb:add_child(btn)
	btn.on_lclick = function(self)
		self.father:close()
	end
end

function test_class()
	Animal = class()

	function Animal:__tostring()
	  return self.name..': '..self:speak()
	end
	function Animal:init(name)
		self.name = name
	end

	Dog = class(Animal)

	function Dog:speak()
	  return 'bark'
	end

	Cat = class(Animal)

	function Cat:init(name,breed)
		Animal.init(self,name)
		self.breed = breed
	end
	function Cat:speak()
	  return 'meow'
	end

	Lion = class(Cat)

	function Lion:speak()
	  return 'roar'
	end

	fido = Dog()
	fido:init('Fido')
	felix = Cat()
	felix:init('Felix','Tabby')
	leo = Lion()
	leo:init('Leo','African')
	print(fido,felix,leo)
end

function test_sort()
	local tb = {
		b = {name = 'b'},
		a = {name = 'a'},
		c = {name = 'c'},
	}
	local t = sort_pairs(tb,"name")
	table_print(t)
end

function test_stringex()
	print(split2("lala.haha","%."))
	split("a,b,c", ",") --> {"a", "b", "c"}
end

function test_tableex()
	table_print({44,{name = "www",sex = 1},func = {1,2,3}})
	local t = {1, {2, 3, 4}, default = {"a", "b", d = {12, "w"}, e = 14}}
	print(table.show(t, "t"))            -- most typical use
end


