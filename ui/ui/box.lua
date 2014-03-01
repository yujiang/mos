--for 绘制一个矩形
local box = class(cell,"box")

function box:create_box(name,x,y,z,w,h,color,alpha)
	cell.create_cell(self,name,x,y,z,w,h)
	self:set_color(color)
	self:set_alpha(alpha)
	--print("box:create_box",self.color,self.alpha)
	self.is_box = true
	self._disable = true
end

return box