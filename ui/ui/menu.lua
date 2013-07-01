--for menu
local menu = class(cell,"menu")

function menu:create_menu_texts(name,x,y,z,w,htext,texts)
	cell.create_cell(self,name,x,y,z,w,htext * #texts)

	--self:set_box(-1,200)
	self.status_box_colors = {-1,0x008080ff,0x00404080}
	self.item_num = #texts

	for i,text in ipairs(texts) do
		local item = mitem()
		item:create_mitem(i,w,htext,text)
		self:add_child(item)

		item.status_box_colors = self.status_box_colors
		item.on_lclick = function(btn) 
			self:on_lclick(btn) 
			self:hide() 
		end
		--bn.on_lclick = menu.on_lclick
	end
end

function menu:create_child_menu(w,htext,texts)
	self:create_menu_texts("menu",0,0,0,w,htext,texts)
end

function menu:on_lclick(btn)
	print("menu:on_lclick",btn.name) 	
end

function menu:get_item(index)
	return self:find_child(tostring(index))
end

function menu:find_item_by_text(name)
	for i = 1,self.item_num do
		local item = self:get_item(i)
		--print(i,item:get_caption_string(),name)
		if item and item:get_caption_string() == name then
			return item
		end
	end
end

function menu:set_check(name,check)
	local item = self:find_item_by_text(name)
	if item then
		item:set_check(check)
	end
end

function menu:in_rect(x,y)
	if cell.in_rect(self,x,y) then
		return true
	end
	local x2 = x 
	local y2 = y 
	for i=1,self.item_num do
		local item = self:get_item(i)
		if item:in_rect(x2,y2) then
			return true
		end
	end
end

--there will be function create_menu_images()...


return menu