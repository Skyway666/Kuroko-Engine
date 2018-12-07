


class EngineComunicator{
	construct new(){}
	foreign static consoleOutput(message)
}

class console_test{
	speed{ _speed }
	speed=(v) {_speed = v }
	static Update(){
		EngineComunicator.consoleOutput("Hello world from a wren function")
	}

	addition(a, b){
		return a + b
	}
}



