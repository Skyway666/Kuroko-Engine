
class ObjectComunicator{
	foreign static C_setPos(gameObject, x, y, z)
	foreign static C_modPos(gameObject, x, y, z)
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