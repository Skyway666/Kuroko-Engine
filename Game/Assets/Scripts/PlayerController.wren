
import "ObjectLinker" for ObjectLinker,
EngineComunicator,
InputComunicator

//For each var you declare, remember to create
//		setters [varname=(v) { __varname = v }]
//		and getters [varname { __varname }]
//The construct method is mandatory, do not erase!
//The import statement at the top og the cript is mandatory, do not erase!
//Be careful not to overwrite the methods declared in Game/ScriptingAPI/ObjectLinker.wren
//[gameObject] is a reserved identifier for the engine, don't use it for your own variables

class PlayerController is ObjectLinker{

    SetState = (new_state) {
        _player_state = new_state   
    }

    MovingState {__moving_state}
    IdleState {__idle_state}

    construct new(){}

    Start() {
        //var punch_var = BasicAttackState.new()
        _player_state

        __idle_state = IdleState.new(this)
        __punch1_state = BasicAttackState.new(this)
        __moving_state = MovingState.new(this)

        _player_state = __idle_state
        _player_state.HandleInput()
    }

    Update() {
        _player_state.HandleInput()
        _player_state.Update()
    }
}

class State {
    GetTotalDuration {_total_duration}
    GetCurrentTime {_current_time_in}

    HandleInput() {

    }

    Update() {
        EngineComunicator.consoleOutput("Base state")
    }
}

class IdleState is State {
    construct new() {
    }

    construct new(player) {
        _player = player
    }

    HandleInput() {
        if(InputComunicator.getKey(InputComunicator.UP, InputComunicator.KEY_DOWN)){
            _player.SetState = _player.MovingState
        }
    }

    Update() {
        EngineComunicator.consoleOutput("Idle state")
    }

}

class MovingState is State {
    construct new(player) {
        _player = player
    }

    HandleInput() {
        if(InputComunicator.getKey(InputComunicator.UP, InputComunicator.KEY_UP)){
            _player.SetState = _player.IdleState
        }
    }
    
    Update() {
        EngineComunicator.consoleOutput("Moving state")
    }
}

class AttackState is State {
    construct new(player) {
        _player = player
    }
}

class BasicAttackState is AttackState {
    construct new(player) {
        _player = player
    }

    construct Fill(animation) {
        _animation = animation
    }

    HandleInput() {
        EngineComunicator.consoleOutput("Estoy handeleando el input que flipas como atake que soy")
    }
}