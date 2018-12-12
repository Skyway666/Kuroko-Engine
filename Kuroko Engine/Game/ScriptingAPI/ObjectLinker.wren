
class ObjectComunicator{
	foreign static C_setTransform(gameObject, x, y, z)
}

class ObjectLinker{
	gameObject { _gameObject}		// UUID of the linked GO
	gameObject=(v){ _gameObject = v}

	setTransform(x,y,z){
		ObjectComunicator.C_setTransform(gameObject, x, y, z)
	}
}