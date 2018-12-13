import "ObjectLinker" for ObjectLinker


class EngineComunicator{
	construct new(){}
	foreign static consoleOutput(message)
}

class console_test is ObjectLinker{

	spe { __speed }
	spe=(v) { __speed = v }

    spwe { __spe }
	spwe=(v) { __spe = v }

	addition(a, b){
		return a + b
	}
	construct new(){}

	Update(){
		EngineComunicator.consoleOutput("My attached GO is %(gameObject)")
		setTransform(3,0,1)
	}
}



