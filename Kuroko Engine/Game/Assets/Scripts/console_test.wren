System.print("Hello, world!")

class Wren {
  flyTo(city) {
    System.print("Flying to ", city)
  }
}

var adjectives = Fiber.new {
  ["small", "clean", "fast"].each {|word| Fiber.yield(word) }
}

while (!adjectives.isDone) System.print(adjectives.call())


class EngineComunicator{
	construct new(){}
	foreign static consoleOutput(message)
	foreign consoleOutput(message)
}

EngineComunicator.consoleOutput("Hello world from C (static)")

var engine_comunicator_instance = EngineComunicator.new()
engine_comunicator_instance.consoleOutput("Hello world from C (instance)")


