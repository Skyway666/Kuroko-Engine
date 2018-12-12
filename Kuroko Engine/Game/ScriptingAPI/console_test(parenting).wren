import "ObjectLinker" for ObjectLinker


class EngineComunicator{
	construct new(){}
	foreign static consoleOutput(message)
}

class console_test is ObjectLinker{

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



