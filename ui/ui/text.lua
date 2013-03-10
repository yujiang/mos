--for ÎÄ×ÖÏÔÊ¾
local text = class(cell,"text")

function text:create_text(name,w,h,s,font,wrap,align)
	cell.create_cell(self,name,0,0,0,w,h)
	--self.is_text = true
	self.font = font or 0
	self.wrap = wrap or 0
	self.align = align or 0
	self.line = cdriver.get_text_line(s,self.font,self.wrap,self.w,self.h)
	self:set_string(s)
end

function text:set_string(s)
	self.text = s
	self.line = self:get_line()
	self.text_w, self.text_h = self:get_text_size()
end

function text:get_line()
	return cdriver.get_text_line(self.text,self.font,self.wrap,self.w,self.h)
end

function text:get_text_size()
	return cdriver.get_text_size(self.text,self.font,self.wrap,self.w,self.h)
end

function text:get_render_override()
	--print("image:get_render_override()",self.name)
	return #self.text > 0 and self or nil
end

function text:set_scroll(scroll)
	--print("image:get_render_override()",self.name)
	self.scroll = scroll
end

function text:in_rect(x,y)
	return x >= 0 and x < self.w and y >= 0 and y < self.h
end

function text:on_mouse_msg(mouse_event,x,y,param)
	if mouse_event == WM_MOUSEWHEEL and self.scroll then
		local miny = self.h - self.line * ui:get_font_height(self.font)
		--print("text:on_mouse_msg",mouse_event,x,y,param)
		--print("h,line,height",self.h,self.line,ui:get_font_height(self.font),miny)
		if miny < 0 then
			local depth = param / 120
			self.y = self.y + self.scroll * depth
			if self.y < miny then
				self.y = miny
			elseif self.y > 0 then
				self.y = 0
			end
		end
		return self
	end
end

return text