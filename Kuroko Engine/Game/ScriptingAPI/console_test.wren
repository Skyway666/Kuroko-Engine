

class EngineComunicator{
	construct new(){}
	foreign static consoleOutput(message)
}

class console_test{

	speed { __speed }
	speed=(v) { __speed = v }

	addition(a, b){
		return a + b
	}
	construct new(){}

	Update(){
		EngineComunicator.consoleOutput("My speed is %(speed)")
	}
}



