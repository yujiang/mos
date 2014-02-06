--$Id: ldb.lua 81730 2009-05-13 10:08:53Z chenjia $
--__auto_local__start--
local string=string
local table=table
local io=io
local debug=debug
local pairs=pairs
local ipairs=ipairs
local tostring=tostring
local tonumber=tonumber
--__auto_local__end--
-- vi: set tabstop=4 shiftwidth=4 noexpandtab :

-- require('traceback')
ldb = class("db")

function ldb:init()
	self.fncache = {}
	self.breaks = {}
	self.bplist = {}
	self.savedhook = {}
	--self.stopframe = nil
	--self.returnframe = nil
end

local ANY_FRAME = -32767
--[[
local _assert = assert
function assert(v, msg)
	if not v then
		Log(debug.traceback())
		_assert(v,msg)
	else
		return v,msg
	end

end
]]

local function Log(...)
	print(...)
end

function _RUNTIME_ERROR(...)
	print(...)
end

function ldb:dispatch( frame, event, line )
	if self.curframe and frame < self.curframe then return end

	if event == 'line' then
		local info = traceback.getinfo( frame, 'Sl')
		return self:dispatch_line( frame, line)
	elseif event == 'call' then
		return self:dispatch_call( frame )
	elseif event == 'return' then
		return self:dispatch_return( frame )
	end
	assert( 'ldb: unknown debug event: ', event )
end

function ldb:dispatch_line( frame, line )
	if self:stop_here( frame ) or self:break_here( frame, line ) then
		self:user_line( frame, line )
	end
end

function ldb:dispatch_call( frame )
	if self:stop_here( frame ) then
		self:user_call( frame )
	end
end

function ldb:dispatch_return( frame )
	if self:stop_here( frame ) then
		self:user_return( frame )
	end
end

function ldb:break_here( frame, line )
	local info = traceback.getinfo( frame, 'Sl')
	local what, filename = traceback.getsource( frame )
	if line ~= nil and self.breaks[filename] and self.breaks[filename][line] then
		return true
	end
	return false
end

function ldb:stop_here( frame )
	if self.curframe and frame < self.curframe then return false end
	if self.stopframe == frame or self.stopframe == ANY_FRAME then
		local what, filename = traceback.getsource( frame )
		if what ==	'C' then
			return false
		else
			return true
		end
	end
	return false
end


function ldb:set_next( frame )
	self.stopframe = frame
end

function ldb:set_step()
	self.stopframe = ANY_FRAME
end

function ldb:set_return( frame )
	self.stopframe = frame + 1
end

function ldb:set_break( filename, lineno )
	lineno = tonumber(lineno)
	line = linecache.getline(filename, lineno)
	if not line then
		return string.format('Line %s:%s does not exist', filename, tostring(lineno))
	end
	Log( filename , lineno)
	if not self.breaks[filename] then
		self.breaks[filename] = {}
	end
	if not self.breaks[filename][lineno] then
		self.breaks[filename][lineno] = true
	end

	table.insert(self.bplist, {filename, lineno})
end

function ldb:clear_break()
	self.breaks = {}
	self.bplist = {}
end

function ldb:delb_bynum( num )
	num = tonumber(num)
	if self.bplist[num] == nil then return end
	local filename, lineno = self.bplist[num][1], self.bplist[num][2]
	if not self.breaks[filename] then return end
	self.breaks[filename][lineno] = nil
	self.bplist[num] = nil
end

function ldb:get_bplist()
	return self.bplist
end

function ldb:construct()
	self.curframe = nil
	self.traceback = {}
end

function ldb:reset()
	linecache.clear()
	self.breaks = {}
	self.bplist = {}
end

function ldb:forget()
	self.curframe = nil
end

function ldb:prepare(  frame, error )
	if frame == nil then
		frame = -traceback.gettop() + 1
	end
	self.error = error
	self.botframe = frame
	self:do_frame( 0 )
end

function ldb:run( cmd )
	local _, _, command, arg = string.find(cmd, '^([a-z]+)%s*(.*)')
	if command == nil then return end
	local abbr={cont='cont',b='setb',bt='backtrace',x='display',h='help',f='frame',l='list',n='next',p='Log',s='step',h='help',q='quit'}
	if abbr[command] then
		command = abbr[command]
	end
	local func = self[string.format('do_%s',command)]
	if not func then
		Log('no such command.')
		return
	end
	return func(self,arg)
end

function ldb:settrace(arg)
	local debugger = self
	function hook ( event, line)
		local frame = traceback.getindex() + 1
		ldb.dispatch( self, frame, event, line )
	end
	if not arg then
		local _data = self.savedhook[ #self.savedhook ]
		if _data ~= nil and type(_data) == "table" and type(_data.hook) == "function" then
			debug.sethook(_data.hook, _data.mask, _data.count)
		else
			debug.sethook()
		end
		table.remove( self.savedhook, #self.savedhook )
	else
		local _hook, _mask, _count = debug.gethook()
		local _data = { hook = _hook, mask = _mask, count = _count }
		table.insert( self.savedhook, _data )
		debug.sethook(hook, arg)
	end
end

function ldb:do_show( cmd )
	local _, _, name = string.find(cmd, '^([a-z]+)')
	if name == 'stack' then
		traceback.dump()
	elseif name == 'path' then
		Log(sys.repr(sys.path))
	end
end

function ldb:get_backtrace( arg, PrintVar )
	local btinfo = {}
	table.insert(btinfo, 'traceback:')
	local top = traceback.gettop()
	local indicator = ' '

	for frame = self.botframe, -1 do
		local level = ( top + 1 ) + frame
		local info = debug.getinfo(level,'nfSlu')
		if info == nil then
			break
		end

		if frame == self.curframe then
			indicator = '<'
		else
			indicator = ' '
		end

		local i = frame - self.botframe
		if info.what == 'C' then   -- is a C function?
			if info.name ~= nil then
				table.insert(btinfo, string.format('\t%s%2d[C] : in %s',indicator, i, info.name))
			else
				table.insert(btinfo, string.format('\t%s%2d[C] :',indicator, i))
			end
			--Log(string.format('\t%s%2d[C]',indicator, i))
		else   -- a Lua function
			if info.name ~= nil then
				table.insert(btinfo , string.format('\t%s%2d %s:%d in %s',indicator, i, info.source, info.currentline, info.name))
			else
				table.insert(btinfo, string.format('\t%s%2d %s:%d',indicator, i, info.source, info.currentline))
			end

			if PrintVar then
				--打印变量
				local values, flags = self:getlocals( frame )
				for k,v in pairs(values) do
					local function SafeGetMsg()
						local Msg = ""
						if type(v) == "table" then
							if v.IsPlayer and v:IsPlayer() then
								Msg = Msg..string.format("userid=%d,name=%s,URS=%s, CorrectClient=%s", v:GetId() or 0,
										v:GetName() or "unknow", v:GetURS() or "unknow", tostring(CorrectClient) )
							elseif v.IsClass then
								local Id = "none"
								if v.GetId and type(v.GetId) == 'function' then
									Id = v:GetId()
								end
								v = string.format("objectid=%s", tostring(Id))
							else
								--v = sys.repr(v) --信息太多了
							end
						end
						return Msg
					end
					_,Msg = xpcall(SafeGetMsg, function(Error) _RUNTIME_ERROR("getmsg fatal error", Error, debug.traceback()) end)
					Msg = string.format("\t\t%s:%s	 %s",k, string.sub(tostring(v),1,50), tostring(Msg))
					table.insert(btinfo, Msg)
				end
			end
		end
	end
	return table.concat(btinfo, "\n")
end

function ldb:do_backtrace(arg, PrintVar)
	local btinfo = self:get_backtrace( arg , PrintVar)
	Log(self.error, btinfo)
end

function ldb:do_do( arg )
	--local _, _, command = string.find(arg,'^.*$')
	loadstring( arg )()
end

local dbg = nil
function ldb.debug( frame, error , active)
	if frame == nil then
		frame = traceback.gettop() + 1
	end
	dbg:prepare(frame,error)
	--debug status
	if active then
		dbg:interaction( frame )
	else
		debug.sethook() --不要hook了。
		local btinfo = dbg:get_backtrace(nil, true)
		dbg:do_quit()
		return btinfo
	end
end

function ldb:do_frame(arg)
	local _, _, frame = string.find( arg, '^(%d+)$')
	if frame == nil then return end
	self.curframe = self.botframe + frame
	if self.curframe >-1 then
		self.curframe = -1
	end
	linecache.clear()
	local what, filename, lineno = traceback.getsource( self.curframe )
	self.curfilename = filename
	self.curlineno = lineno - 5
end


function ldb:do_help( arg )
	local msg =
	{
		'backtrace(bt)		   -- show backtrace',
		'frame(f)			   -- switch to certain frame',
		'list(l) <file> <line> -- list the source file',
		'display(p)			   -- show content of expr',
		'Log(p)				 -- show value of expr',
		'set var = exp		   -- change var\'s value',
		'do statement		   -- execute statement',
		'info locals		   -- show local variables',
		'show path			   -- show search path',
		'show stack			   -- show full call tracebacks',
		'setb(b) <file> <line> -- sets a breakpoint',
		'delb <num>			   -- removes a breakpoint',
		'clearb				   -- removes all breakpoints',
		'continue(cont)			  -- run until next breakpoint, will not clear debug hook',
		'step(s)			   -- run until next line, stepping into function calls',
		'next(s)			   -- run until next line, stepping over function calls',
		'return(r)			   -- run until return to the caller',
		'breaks				   -- lists breakpoints',
		'path [<path>]		   -- sets the base path of the remote application, or shows the current one',
		'quit				   -- quit debugger, and clear debug hook',
		'help(h)			   -- help',
	}
	Log(table.concat(msg,'\n'))
end

function ldb:getlocals( frame )
	local i = 1
	local values = {}
	local flags = {}
	frame = traceback.gettop() + 1 + frame
	local info = debug.getinfo(frame)
	if info == nil then return end

	if info.func then
		i = 1
		while true do
			local name, value = debug.getupvalue(info.func,i)
			if not name then break end
			flags[name] = true
			values[name] = value
			i = i + 1
		end
	end

	i = 1
	while true do
		local name, value = debug.getlocal( frame, i)
		if not name then break end
		flags[name] = true
		values[name] = value
		i = i + 1
	end

	return values, flags
end

function ldb:getglobals()
	local values = {}
	local flags = {}
	for name, value in pairs(_G) do
		if not flags[name] then
			flags[name] = true
			values[name] = value
		end
	end
	return values, flags
end

function ldb:setvar( frame, var, exp)
	frame = traceback.gettop() + 1 + frame
	local info = debug.getinfo(frame)
	if info == nil then return end

	local i
	i = 1
	while true do
		local name, value = debug.getlocal( frame, i)
		if not name then break end
		if name == var then
			debug.setlocal( frame, i, exp)
			return true
		end
		i = i + 1
	end

	if info.func then
		i = 1
		while true do
			local name, value = debug.getupvalue(info.func,i)
			if not name then break end
			if name == var then
				debug.setlocal( frame, i, exp)
				return true
			end
			i = i + 1
		end
	end

	for name, value in pairs(_G) do
		if name == var then
			_G[var] = exp
			return true
		end
	end
	return false
end

function ldb:setvars( frame, flags, values)
	while frame <=0 do
		for k, v in pairs(flags) do
			local var = k
			local exp = values[k]
			local result = self:setvar( frame, var, exp)
			if result then flags[k] =  false end
		end
		frame = frame + 1
	end
end


function ldb:do_info( arg )
	local info = traceback.getinfo(self.curframe)
	if info == nil then return end
	local _, _, command = string.find(arg, '^([%w]+)')
	if command == 'locals' then
		local values, flags = self:getlocals( self.curframe )
		if values ~= nil then
			for name, _ in pairs(flags) do
				Log(string.format('%s = %s', name, sys.repr(values[name])))
			end
		end
	end
end


function ldb:do_list( arg )
	local filename, lineno

	local args = {}
	for w in string.gmatch(arg, '[^%s]+') do
		table.insert(args,w)
	end
	if #args == 1 then
		if string.match(args[1],'^%d+$') then
			filename = self.curfilename
			lineno = args[1]
		elseif string.match(args[1],'^[ ]*-[ ]*$') then
			filename = self.curfilename
			lineno = self.curlineno - 20
		else
			filename = args[1]
			lineno = self.curlineno
		end
	elseif #args == 2 then
		filename = args[1] or self.curfilename
		lineno = args[2] or self.curlineno
	else
		filename = self.curfilename
		lineno = self.curlineno
	end
	filename = self:findfile( filename )
	if lineno == 0 then lineno = 1 end

	for i = 0, 9 do
		local line = linecache.getline( filename, lineno + i )
		if not line then break end
		Log(string.format('%3d	%s', lineno + i, line))
	end
	self.curfilename = filename
	self.curlineno = lineno + 10
end

function ldb:do_path( arg )
	local _, _, path = string.find(arg, '^([^ ]+)')
	if path == nil then
		sys.path = {}
	else
		table.insert(sys.path, 1, path)
	end
	linecache.clear()
end

function ldb:do_Log( arg )
	local _, _, statement = string.find(arg,'^(.*)$')
	local result, flags, values = self:exec( self.curframe, 'return {' .. statement .. '}')

	if result ==nil or type(result) ~= 'table' or #result == 0 then
		Log('nil')
	else
		local msg = sys.repr(result)
		Log(string.sub(msg,2,-3))
	end
end

function ldb:do_display( arg )
	local _, _, statement = string.find(arg,'^(.*)$')
	local result, flags, values = self:exec( self.curframe, 'return {' .. statement .. '}')

	if result ==nil or type(result) ~= 'table' or #result == 0 then
		Log('nil')
	else
		for k, v in pairs( result ) do
			local msg = sys.repr(v)
			Log(msg)
		end
	end
end

function ldb:exec( frame, statement )
-- get variables stack
	local locate = frame
	local sentinel_meta =
	{
		values = {},
		flags  = {},
		vars_nil = {},
		__newindex = function( t, k, v )
			local self = getmetatable(t)
			self.flags[ k ] = true
			self.values[k] = v
		end,

		__index = function( t, k )
			local self = getmetatable(t)
			if self.vars_nil[k] == true then return nil end
			if self.flags[k] then
				return self.values[k]
			else
				if self.__next then
					return self.__next[k]
				else
					return nil
				end
			end
		end
	}

	local sentinel = {}
	setmetatable( sentinel, sentinel_meta )

	local vars_stack = {}
	local vars_nil = {}
	while locate < 0 do
		local info = traceback.getinfo(locate,'f')
		if info == nil then break end
		local values, flags = self:getlocals(locate)
		if values == nil then break end
		for k, v in pairs(flags) do
			if values[k] == nil then
				vars_nil[k] = true
			end
		end
		table.insert(vars_stack,values)
		locate = locate + 1
		break
	end
	sentinel_meta.vars_nil = vars_nil

	local info = traceback.getinfo(frame,'f')
	if info and info.func then
		local env = getfenv( info.func )
		table.insert(vars_stack,env)
	end

	-- link them
	for i = 1, #vars_stack - 1 do
		local curr_stack = vars_stack[i]
		local next_stack = vars_stack[i+1]
		setmetatable( curr_stack, {__index = next_stack} )
	end

	-- link sentinel
	sentinel_meta.__next = vars_stack[1]

	-- execute
	local func = loadstring(  statement )
	if func == nil then
		Log('invalid statement.')
		return
	end

	setfenv(func, sentinel)
	local result = func()

	return result, sentinel_meta.flags, sentinel_meta.values
end

function ldb:do_set( arg )
	local _, _, statement = string.find(arg,'^(.*)$')
	local result, flags, values = self:exec( self.curframe, statement )
	self:setvars(self.curframe, flags, values)
end

function ldb:findfile( filename)
	for _, v in ipairs(sys.path) do
		local name = os.path.normpath(os.path.join(v,filename))
		if os.path.exists(name) then
			return name
		end
	end
	return nil
end

function ldb:do_setb( arg )
	local _, _, filename, lineno = string.find(arg, '^([%w%p]+)%s+(%d+)$')
	if not ( filename and lineno ) then
		return
	end

	local target = self:findfile( filename )
	if target == nil then
		Log(string.format('%s not exist.', filename))
		return
	end
	local result = self:set_break(target,lineno)
	if result then
		Log(result)
	end
end

function ldb:do_delb(arg)
	local _,_, num = string.find( arg, '^(%d+)$')
	Log(num)
	self:delb_bynum(num)
end

function ldb:do_do(arg)
	local _, _, statement = string.find(arg,'^(.*)$')
	local result, flags, values = self:exec( self.curframe, statement )
end

function ldb:do_clearb( arg )
	self:clear_break()
end

function ldb:do_breaks(arg)
	local breaks = self:get_bplist()
	Log('break points:')
	for i,v in ipairs(breaks) do
		Log( string.format('%d %s:%s',i, v[1], v[2]))
	end
end



function ldb:do_next( arg )
	local curframe = self.curframe
	local what, filename, lineno = traceback.getsource( curframe )
	Log('set',filename, lineno)
	self:set_next( curframe )
	return true
end

function ldb:do_return( arg)
	local curframe = self.curframe
	self:set_return( curframe )
	return true
end

function ldb:do_step( arg )
	self:set_step()
	return true
end

function ldb:do_cont( arg )
	return true
end

function ldb:do_quit( arg )
	self:settrace()
	self:reset()
	return true
end

function ldb:cmdloop()
	local stop = false
	local readline = nil
	if tty and tty.readline then
		readline = tty.readline
	else
		readline =
		function()
			return io.read('*line')
		end
	end
	local lastcmd = ''
	while( not stop ) do
		io.write('>')
		cmd = readline()

		if cmd == nil then
			cmd = 'quit'
		end

		if cmd == '' then
			cmd = lastcmd
		end
		lastcmd = cmd

		local status = nil
		status, stop = pcall(function () return self:run(cmd) end)
	end
end

function ldb:interaction( frame )
	self:cmdloop()
end

function ldb:pframe( frame )
	local what, filename, lineno = traceback.getsource( frame )
	local line = linecache.getline( filename, lineno )
	if what ~= 'Lua' then
		Log(string.format(' %s', tostring(filename) ))
	elseif line then
		Log(string.format(' %3d  %s', lineno, line))
	end
end

function ldb:user_line( frame, line )
	self:prepare( frame )
	self:pframe( frame )
	self.stopframe = nil
	self:interaction(frame)
end

function ldb:user_call( frame )
	Log('--call--')
	self:prepare( frame )
	self:pframe( frame )
	self:interaction(frame)
end

function ldb:user_return( frame )
	Log('--return--')
	self:prepare( frame )
	self:pframe( frame )
	self:interaction(frame)
end

local function OnExcept( Error , active, n)
	local curframe = -traceback.gettop() --curframe
	--防止栈溢出
	if curframe < -150 then
		return "stack too deep!!\n" .. debug.traceback()
	end
	local botframe = curframe + 1 + (n or 1) -- trackback, caller
	--print(botframe,active,n)
	local btinfo = ldb.debug( botframe, Error, active)
	return btinfo
end

function SafeExcept(Error)
	_RUNTIME_ERROR("SafeExcept!", Error )

	local btinfo = OnExcept(Error, false, 3)
	if not btinfo then
		_RUNTIME_ERROR("no btinfo", type(Err), Err, type(btinfo), btinfo)
	else
		_RUNTIME_ERROR(btinfo)
	end

	dbg:do_quit()
	debug.sethook()

	return Error
end

function init_dbg()
	if dbg == nil then
		dbg = ldb()
		dbg:init()
		dbg:reset()
		dbg:settrace('lcr')
		dbg:do_quit()
	end
end

init_dbg()
debug.excepthook = SafeExcept

--[[
-- 说明：
-- 在代码中强制进入调试器：
-- ldb.debug(-traceback.gettop())
--]]
