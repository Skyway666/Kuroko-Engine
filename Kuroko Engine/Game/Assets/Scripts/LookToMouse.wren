
import "ObjectLinker" for ObjectLinker, EngineComunicator, InputComunicator

class LookToMouse is ObjectLinker{
	construct new(){}

	Update(){		
		lookAt(InputComunicator.getMouseRaycastX() , 0, InputComunicator.getMouseRaycastZ() )
	}
}

