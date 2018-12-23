
import "ObjectLinker" for ObjectLinker, EngineComunicator, InputComunicator

class LookToMouse is ObjectLinker{
	construct new(){}

	Update(){		
		lookAt(InputComunicator.getMouseRaycastX() , getPosY("local"), InputComunicator.getMouseRaycastZ() )
	}
}

