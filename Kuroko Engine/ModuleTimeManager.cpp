#include "ModuleTimeManager.h"
#include "Applog.h"




ModuleTimeManager::ModuleTimeManager(Application * app, bool start_enabled): Module(app, start_enabled)
{
	name = "time_manager";
	
}



ModuleTimeManager::~ModuleTimeManager()
{

}

bool ModuleTimeManager::Init(const JSON_Object * config)
{
	// Set clock configuration in some way?


	return true;
}

update_status ModuleTimeManager::Update(float dt)
{
	switch (game_state) {
	case GameState::PLAYING:
		ManageGamePlaying();
		break;
	case GameState::PAUSED:

		break;
	
	case GameState::STOPPED:
		break;
	}

	game_time.Update();
	return UPDATE_CONTINUE;
}

bool ModuleTimeManager::CleanUp()
{

	return true;
}

void ModuleTimeManager::Advance() {
	// Should only allow to advance if the game is paused
}



void ModuleTimeManager::ManageGamePlaying() {

	bool state_changed = false;

	switch (last_game_state) {
	case PAUSED:
		game_time.Resume();
		state_changed = true;
		break;
	case STOPPED:
		real_time.Start();
		game_time.Start();
		state_changed = true;
		break;
	}

	if (state_changed)
		last_game_state = game_state;
		 
}

void ModuleTimeManager::ManageGamePaused() {
	bool state_changed = false;

	switch (last_game_state) {
	case PLAYING:
		game_time.Pause();
		state_changed = true;
		break;
	case STOPPED:
		app_log->AddLog("Can't pause game while stopped!");
		game_state = STOPPED;
		break;
	}

	if(state_changed)
		last_game_state = game_state;
}

void ModuleTimeManager::ManageGameStopped() {
	bool state_changed = false;

	switch (last_game_state) {
	case PLAYING:
	case PAUSED:
		real_time.Reset();
		game_time.Reset();
		state_changed = true;
		break;
	}

	if (state_changed)
		last_game_state = game_state;
}

void GameTimer::Start() {
	paused = false;
	last_frame_time = SDL_GetTicks();
	time = 0;
}

void GameTimer::Update() {
	if (!paused) {
		delta_time = (SDL_GetTicks() - last_frame_time)* time_scale;
		time += delta_time;
	}
	else
		delta_time = 0;

	last_frame_time = SDL_GetTicks();
}

void GameTimer::Pause() {
	paused = true;
}

void GameTimer::Resume() {
	paused = false;
}

void GameTimer::Reset() {
	time = 0;
	paused = true;
}

float GameTimer::Read() {
	return time;
}

void GameTimer::setScale(float scale) {
	time_scale = scale;
}