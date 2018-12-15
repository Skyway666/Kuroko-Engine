import "ObjectLinker" for ObjectLinker, EngineComunicator, InputComunicator

class Move is ObjectLinker{
	construct new(){}

	speed { __speed }
	speed=(v) { __speed = v }

	Update(){
		if(InputComunicator.getKey(InputComunicator.UP)){
			modPos(0,0,-speed)
		}
		
		if(InputComunicator.getKey(InputComunicator.DOWN)){
			modPos(0,0,speed)
		}

		if(InputComunicator.getKey(InputComunicator.LEFT)){
			modPos(-speed,0,0)
		}
		
		if(InputComunicator.getKey(InputComunicator.RIGHT)){
			modPos(speed,0,0)
		}
	}
}



