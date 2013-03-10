-- class.lua
-- Compatible with Lua 5.1 (not 5.0).

-- from lua wiki http://lua-users.org/wiki/SimpleLuaClasses
-- 没有用其他的方法是因为这个足够简单
-- 去掉隐式调用的init，改为必须显示调用，这样也好有init1，init2

function class(base,class_name)
   local c = {}    -- a new class instance

   if class_name == nil then
	  class_name = base
	  base = nil
   elseif type(base) == 'table' then
    -- our new class is a shallow copy of the base class!
      for i,v in pairs(base) do
         c[i] = v
      end
      c._base = base
   end
   -- the class will be the metatable for all its objects,
   -- and they will look up their methods in it.
   c.__index = c

   local mt = {}
   mt.__call = function()
	   local obj = {}
	   setmetatable(obj,c)
	   obj.class_name = class_name
	   return obj
   end

   c.is_a = function(self, klass)
      local m = getmetatable(self)
      while m do 
         if m == klass then return true end
         m = m._base
      end
      return false
   end

   setmetatable(c, mt)
   return c
end

