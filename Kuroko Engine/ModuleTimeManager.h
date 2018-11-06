#ifndef __MODULE_TIME_MANAGER
#define __MODULE_TIME_MANAGER

#include "Module.h"
#include "Timer.h"

enum GameState { STOPPED, PAUSED, PLAYING };


class GameTimer {


public:
	GameTimer() {};
	void Start();
	void Update(); // Allways update, time is managed through pause/resume
	void Pause();
	void Resume();
	void Reset();
	float Read();
	void setScale(float scale);


private:
	// All in ms
	float time = 0;
	float time_scale = 0;
	float last_frame_time = 0;
	float delta_time = 0;

	bool paused = true;

};

class ModuleTimeManager: public Module
{
public:
	ModuleTimeManager(Application* app, bool start_enabled = true);
	~ModuleTimeManager();

	bool Init(const JSON_Object* config);
	update_status Update(float dt);
	bool CleanUp();

	void Play() { game_state = PLAYING; };
	void Pause() { game_state = PAUSED; };
	void Stop() { game_state = STOPPED; };
	void Advance();
	GameState getGameState() { return game_state; };

private:


	void ManageGamePlaying();
	void ManageGamePaused();
	void ManageGameStopped();

	GameState game_state = GameState::STOPPED;
	GameState last_game_state = GameState::STOPPED;
	float time_scale;

	int frame_count = 0;
	Timer real_time;
	GameTimer game_time;

	float real_dt;
	float game_dt;
};


#endif