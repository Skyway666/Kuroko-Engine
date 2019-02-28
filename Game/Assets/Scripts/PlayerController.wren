
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

    construct new(){}

    Start() {
        //var punch_var = BasicAttackState.new()
        var player_state

        __punch1_state = BasicAttackState.new()
        __moving_state = MovingState.new()

        player_state = __punch_att
        player_state = __moving_state
        player_state.HandleInput()
    }

    Update() {

    }
}

class State {
    GetTotalDuration {_total_duration}
    GetCurrentTime {_current_time_in}

    construct new() {
        EngineComunicator.consoleOutput("Soc un putu deu del wren")
    }

    HandleInput() {
        EngineComunicator.consoleOutput("Estoy handeleando el input que flipas")
    }

    Update() {

    }
}

class IdleState is State {

}

class MovingState is State {
    construct new() {

    }

    HandleInput() {
        EngineComunicator.consoleOutput("Estoy handeleando el input que flipas como moviemiento que soy")
    }
}

class AttackState is State {
    construct new() {
        super()
        EngineComunicator.consoleOutput("attack new")
    }
}

class BasicAttackState is AttackState {
    construct new() {
        super()
        EngineComunicator.consoleOutput("battack new")
    }
    construct Fill(animation) {
        _animation = animation
    }

    HandleInput() {
        EngineComunicator.consoleOutput("Estoy handeleando el input que flipas como atake que soy")
    }
}