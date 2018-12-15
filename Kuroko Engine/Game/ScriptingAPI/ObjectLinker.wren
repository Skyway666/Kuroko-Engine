
class ObjectComunicator{
	foreign static C_setPos(gameObject, x, y, z)
	foreign static C_modPos(gameObject, x, y, z)
}

class EngineComunicator{
	foreign static consoleOutput(message)
}

class InputComunicator{
	
	static UP {82}
	static DOWN {81}
	static LEFT {80}
	static RIGHT {79}
	foreign static getKey(key)
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
}