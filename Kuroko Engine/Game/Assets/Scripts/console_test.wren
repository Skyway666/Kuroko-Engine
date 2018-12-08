


class EngineComunicator{
	construct new(){}
	foreign static consoleOutput(message)
}

class console_test{

	static speed { __speed }
	static speed=(v) { __speed = v }

	speed { __speed }
	speed=(v) { __speed = v }

	addition(a, b){
		return a + b
	}
	static Update(){
		EngineComunicator.consoleOutput("My speed is %(speed)")
	}
}



