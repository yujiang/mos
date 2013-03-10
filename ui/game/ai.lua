--运动
local ai = class("ai")

function ai:create_ai(sprite)
	self.sprite = sprite
end

function ai:stop_ai()
	if self.on_reach then
		self.on_reach.invalid = true
	end
end

function ai:move_ai(func)
	--stop_ai()用于2种ai交叉执行。
	self:stop_ai()

	local move = self.sprite:get_move()
	move:stop_move() 

	local co = coroutine.create(func)
	self.on_reach = self.sprite.emitter:on("reach",
		function(is_reached)
			--print("ai:walk_random",is_reached)
			return self:on_reached(co,is_reached)
		end)
	return co	
end

function thread_walk_path(sprite,path,speed,loop)
	repeat
		for index,pos in ipairs(path) do
			sprite:walk_to(pos[1],pos[2],speed)
			local is_reached = coroutine.yield(index)
			if not is_reached then
				--print("ai.thread_walk_path",is_reached)
				print(debug.traceback())
				return
			end
		end
	until(not loop)
end

function ai:on_reached(co,is_reached)
	local rt,index = coroutine.resume(co,is_reached)
	--print("ai:on_reached",rt,index,is_reached)
	if not rt then
		return false
	end
	return is_reached
end

function ai:walk_path(path,speed,loop)
	local co = self:move_ai(thread_walk_path)
	local rt,index = coroutine.resume(co,self.sprite,path,speed,loop)
	--print("ai:walk_path",rt,index)
end

function thread_walk_random(sprite,range,speed)
	local x,y = sprite:get_pos()
	local index = 1
	while(1) do
		local x1 = x + math.random(-range,range)
		local y1 = y + math.random(-range,range)
		if x1 < 0 then x1 = 0 end
		if y1 < 0 then y1 = 0 end
		sprite:walk_to(x1,y1,speed)
		local is_reached = coroutine.yield(index)
		index = index + 1
	end
end

function ai:walk_random(range,speed)
	local co = self:move_ai(thread_walk_random)
	local rt,index = coroutine.resume(co,self.sprite,range,speed)
	--print("ai:walk_random",rt,index)
end

return ai