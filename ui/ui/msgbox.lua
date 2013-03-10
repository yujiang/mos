
local msgbox = class(window,"msgbox")

function msgbox:create_msgbox(name,x,y,w,h)
	local tb = window.create_window(self,name,x,y,0,w,h)
	self.drag = true

	g_root:set_modal_window(self)

	self.is_dynamic = true
	return tb
end

return msgbox