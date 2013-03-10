
local layer = class(cell,"layer")

function layer:create_layer(name,z,w,h)
	cell.create_cell(self,name,0,0,z,w,h)
	self.is_layer = true
end

function layer:add_window(w)
	self:add_child(w)
	if self.name == "normal" and w:is_show() then
		self.father:add_chain(w)
	end
end

function layer:show_all_window()
	for name,win in pairs(self.childs) do
		if win:is_hide() then
			win:show()
		end
	end	
end

function layer:get_all_window_name(t)
	for name,win in pairs(self.childs) do
		if win:is_hide() then
			table.insert(t,win.name)
		end
	end	
end

function layer:show_window(name)
	if self:find_child(name) then
		self:find_child(name):show()
	end
end

return layer
