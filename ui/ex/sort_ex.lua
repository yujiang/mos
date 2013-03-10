--require "table_ex"

function sort_pairs(tb,name)
	local t = {}
	for k,v in pairs(tb) do
		table.insert(t,v)
	end
	table.sort(t,function(a,b) return a[name] < b[name] end)
	return t
end

--test()