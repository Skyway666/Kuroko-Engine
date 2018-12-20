
class ObjectComunicator{
	foreign static C_setPos(gameObject, x, y, z)
	foreign static C_modPos(gameObject, x, y, z)
	foreign static C_lookAt(gameObject, x, y, z)
	foreign static C_getPosX(gameObject)
	foreign static C_getPosY(gameObject)
	foreign static C_getPosZ(gameObject)
	foreign static C_Kill(gameObject)
}

class EngineComunicator{
	foreign static consoleOutput(message)
	foreign static Instantiate(prefab_name, x, y, z)
}

class InputComunicator{
	
	static UP {82}
	static DOWN {81}
	static LEFT {80}
	static RIGHT {79}
	static SPACE {44}

	static KEY_DOWN {1}
	static KEY_REPEAT {2}
	foreign static getKey(key, mode)
	foreign static getMouseRaycastX()
	foreign static getMouseRaycastY()
	foreign static getMouseRaycastZ()
}


class ObjectLinker{
	gameObject { _gameObject}		// UUID of the linked GO
	gameObject=(v){ _gameObject = v}

	setPos(x,y,z){
		ObjectComunicator.C_setPos(gameObject, x, y, z)
	}
	modPos(x,y,z){
		ObjectComunicator.C_modPos(gameObject, x, y, z)
	}

	lookAt(x,y,z){
		ObjectComunicator.C_lookAt(gameObject, x, y, z)
	}

	kill(){
	    ObjectComunicator.C_Kill(gameObject)
	}

	getPosX(){
		return ObjectComunicator.C_getPosX(gameObject)
	}

	getPosY(){
		return ObjectComunicator.C_getPosY(gameObject)
	}

	getPosZ(){
		return ObjectComunicator.C_getPosZ(gameObject)
	}
}