import "ObjectLinker" for ObjectLinker,
EngineComunicator,
InputComunicator,
Vec3

//For each var you declare, remember to create
//		setters [varname=(v) { __varname = v }]
//		and getters [varname { __varname }]
//The construct method is mandatory, do not erase!
//The import statement at the top og the cript is mandatory, do not erase!
//Be careful not to overwrite the methods declared in Game/ScriptingAPI/ObjectLinker.wren
//[gameObject] is a reserved identifier for the engine, don't use it for your own variables

class PlayerMove is ObjectLinker{

construct new(){}

//Movement
speed {__speed}
speed=(v){__speed = v}

direction {__direction}
direction=(v){__direction = v}

//Dash
dash_speed {__dash_speed}
dash_speed=(v){__dash_speed = v}

dash_duration {__dash_duration}
dash_duration=(v){__dash_duration = v}

dashing {__dashing}
dashing=(v){__dashing = v}



 Start() {
direction = Vec3.new(0,0,0)
}

 Update() {
//TODO: Update the player's direction with the input and then move him using the speed
//TODO 2:If the player dashes, deactivate movement and make him dash
//Needed: A way to use classes effectively. comunication between scripts
direction = Vec3.new(0,0,0)
//if(InputComunicator.getButton(1, InputComunicator.L_AXIS_UP, InputComunicator.KEY_REPEAT)){
		//	direction.z = 1
		//}
		
//if(InputComunicator.getButton(1, InputComunicator.L_AXIS_DOWN, InputComunicator.KEY_REPEAT)){
//			direction.z = -1
//		}
//if(InputComunicator.getButton(1, InputComunicator.L_AXIS_LEFT, InputComunicator.KEY_REPEAT)){
		//	direction.x = 1
		//}
		
//if(InputComunicator.getButton(1, InputComunicator.L_AXIS_RIGHT, InputComunicator.KEY_REPEAT)){
	//	direction.z = -1
		//}
//var movement = Vec3.new(direction.x*speed,0,direction.z*speed)
//modPos(movement.x,movement.y,movement.z)

if(InputComunicator.getKey(InputComunicator.UP, InputComunicator.KEY_REPEAT)){
			direction.z = 1
		}
		
		if(InputComunicator.getKey(InputComunicator.DOWN, InputComunicator.KEY_REPEAT)){
			direction.z = -1
		}

		if(InputComunicator.getKey(InputComunicator.LEFT, InputComunicator.KEY_REPEAT)){
			direction.x = 1
		}
		
		if(InputComunicator.getKey(InputComunicator.RIGHT, InputComunicator.KEY_REPEAT)){
			direction.x = -1
		}
var movement = Vec3.new(direction.x*speed,0,direction.z*speed)
modPos(movement.x,movement.y,movement.z)

var pos = getPos("global")
var look = direction + pos
lookAt(look.x,0.look.z)
}
}