
class EngineComunicator{
	construct new(){}
	foreign static consoleOutput(message)
	foreign consoleOutput(message)
}

class TestScript{

	speed{ _speed }
	speed=(v) {_speed = v }
	static Update(){
		EngineComunicator.consoleOutput("Hello world from a wren function")
	}

	addition(a, b){
		return a + b
	}
}

EngineComunicator.consoleOutput("Hello world from C (static)")

var engine_comunicator_instance = EngineComunicator.new()
engine_comunicator_instance.consoleOutput("Hello world from C (instance)")

// File modification
