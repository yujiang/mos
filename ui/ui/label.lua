--for Œƒ◊÷œ‘ æ
local label = class(cell,"label")

function label:create_label(name,x,y,z,w,h,s,font,wrap,align)
	cell.create_cell(self,name,x,y,z,w,h)
	--self.text = s
	local t = text()
	t:create_text("text",w,h,s,font,wrap,align)
	--self.childs = {}
	--self.childs["text"] = t
	self:add_child(t)

	self._disable = true;
end

function label:get_text()
	return self.childs["text"]
end

function label:get_string()
	return self:get_text().text
end

function label:set_string(s)
	self:get_text():set_string(s)
	if self.align then
		set_align(self.align)
	end
end

function label:set_align(align)
	assert(align)
	self.align = align
	local text = self:get_text()
	local x = 0
	local y = 0
	if align == ALIGN_CENTER then
		x = (self.w - text.text_w)/2
		y = (self.h - text.text_h)/2
		--print("label:set_align",x,y)
	end
	text:set_pos(x,y)
end


return label