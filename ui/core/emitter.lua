
local emitter = class("emitter")

-- By default, and error events that are not listened for should thow errors
function emitter:missing_handler_type(name, ...)
  --_oldprint("emitter:missing_handler_type")
  if name == "error" then
    error(...)
  end
end

-- Add a new typed event emitter
function emitter:on(name, callback)
  --_oldprint("emitter:on")
  local handlers = rawget(self, "handlers")
  if not handlers then
    handlers = {}
    rawset(self, "handlers", handlers)
  end
  local handlers_for_type = rawget(handlers, name)
  if not handlers_for_type then
    if self.add_handler_type then
      self:add_handler_type(name)
    end
    handlers_for_type = {}
    rawset(handlers, name, handlers_for_type)
  end
  local tb = {callback = callback}
  table.insert(handlers_for_type,tb)
  return tb
end

function emitter:emit(name, ...)
  --_oldprint("emitter:emit")
  local handlers = rawget(self, "handlers")
  if not handlers then
    self:missing_handler_type(name, ...)
    return
  end
  local handlers_for_type = rawget(handlers, name)
  if not handlers_for_type then
    self:missing_handler_type(name, ...)
    return
  end

  local t = {}
  rawset(handlers, name, t)
  for _, v in ipairs(handlers_for_type) do
    if not v.invalid and v.callback(...) then
		table.insert(t,v)
	end
  end
end

return emitter

