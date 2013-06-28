
--得到2个
function split2(s,sign)
	local pos,t = string.find(s,sign)
	--print(pos,t,s,sign)
	if pos then
		return string.sub(s,1,pos-1),string.sub(s,pos+1)
	end
	--return s
end

function split(str, pat)
   local t = {}  -- NOTE: use {n = 0} in Lua-5.0
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
	 table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

function string.starts(String,Start)
   return string.sub(String,1,string.len(Start))==Start
end

function string.ends(String,End)
   return End=='' or string.sub(String,-string.len(End))==End
end

--手写的，感觉不好
function string.rfind(s,pat)
	local m,n
	local len = 0
	while(true) do
		i,j = string.find(s,pat)
		if (j == nil) then
			break
		end
		m,n = i,j
		s = string.sub(s,j+1)
		len = len + j
		--print(s,len)
	end
	if (len == 0) then
		return
	end
	return m+len-n,n+len-n
end

function is_char(int,s)
	return string.byte(s,1) == int
end

