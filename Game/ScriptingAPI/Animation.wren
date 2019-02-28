
class AnimatorComunicator{
	foreign static C_SetAnimation(gameObject, component, sound)
	foreign static C_Play(gameObject, component)
	foreign static C_Pause(gameObject, component)
}
class AnimatorComponent{
	gameObject { _gameObject}		// UUID of the linked GO
	gameObject=(v){ _gameObject = v}

	component { _component}		// UUID of the component 
	component=(v){ _component = v}

	construct new( parent, uuid){
		gameObject = parent
		component = uuid
	}

	setAnimation(animation){
		AnimatorComunicator.C_SetSound(gameObject, component, animation)
	}

	Play(){
		AnimatorComunicator.C_Play(gameObject, component)
	}
	Pause(){
		AnimatorComunicator.C_Pause(gameObject, component)
	}

}