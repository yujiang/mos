抱歉只有中文的说明

特色
1. c++层仅有系统相关的部分（仅仅实现了渲染，并且是gdi渲染）
2. 所有游戏对象和逻辑都在lua
	a. lua的游戏对象，在c++层没有对应物，两边也不通过get set相互设置
	b. 每一帧，所有的渲染对象按照排序组成一个table传到c++，c++遍历整个table来渲染
3. 这样设计的好处
	非常松的耦合
	lua跨平台
	对于使用者来说无需了解c++
	c++只关心系统的话封闭性好；lua只关心逻辑扩展性好
	c++写逻辑需要设计模式；而lua的动态，gc，闭包，写逻辑无需什么设计模式（只有一个observe用到了）
	c++修改代码需要不断编译，时间大把；而lua可以动态重载，调试方便
	存取直接存取lua对象即可
	lua的corroutine写ai和魔法表现，镜头动画等非常符合直觉；c++虽有类似的fiber，但是不是一个等级的东西
	一次性渲染的方式在c++层可以做点合并批次的优化——尚未实现
4. 这样设计的局限
	对象不能太多，否则会慢吧没有测试过，特别适合手机（对象100个左右）

目录结构
mos c++ 
ui lua

开发环境
mos/mos.sln 输出 ui/mos.exe
vs2010，其中对lua的支持在
https://github.com/downloads/yujiang/vsLua/LuaLanguage.vsix

lua的数据结构
g_root
	一棵树，绑定所有的游戏对象（包括ui，地图，精灵）		
g_camera
	摄像机，现在只能对准g_root:get_play()
g_timer
	定时器，支持4种定时器，一般只需要了解2种
	t = add_timer_everyframe(func,param)
	t = add_timer(time,func,param)
	del_timer的方法:
		1. func返回false，timer不再执行
		2. t.invalid = true
g_db
	所有的表格资源。比如action的定义

c++的数据结构
graph
	所有的资源cache
image_db
	定义一个image+frame是来自一个图片的某个rect——常见的是把所有ui资源做到一个大图中。


输入常用命令(在main.lua)
r reload 比如改了一个button；r button，会遍历root，把所有button对象的metatable替换成新的。
l load window	
s show window
df dofile 常用df main.lua 
dump cdriver.dump_resource 看c++的cache
其他执行dostring: test()获得常见测试