
class EngineComunicator{
	construct new(){}
	foreign static consoleOutput(message)
	foreign consoleOutput(message)
}

class TestScript{
	static  Update(){
		EngineComunicator.consoleOutput("Hello world from a wren function")
	}
}

EngineComunicator.consoleOutput("Hello world from C (static)")

var engine_comunicator_instance = EngineComunicator.new()
engine_comunicator_instance.consoleOutput("Hello world from C (instance)")

// File modification
