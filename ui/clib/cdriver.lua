-- monk c++ part so can run from main.lua
-- just check logic no display and msg!
local cdriver = class()

function cdriver.get_image_size(file)
	return 100,100
end

function cdriver.render(rd)
	print("cdriver:render",rd)
	cell.print_render(rd)
end

return cdriver