
class EngineComunicator2{
	construct new(){}
	foreign static consoleOutput(message)
	foreign consoleOutput(message)
}

class TestScript2{
	static  Update(){
		EngineComunicator.consoleOutput("Hello world 2 from a wren function")
	}
}

EngineComunicator.consoleOutput("Hello world from C (static)")

var engine_comunicator_instance2 = EngineComunicator2.new()
engine_comunicator_instance2.consoleOutput("Hello world from C (instance)")

// File modification
