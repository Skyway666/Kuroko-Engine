#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;
struct ImDrawData;


class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:

	p2List<PhysBody3D*> speedway;
	p2List<PhysBody3D*> speedway_sensors;
	p2List<PhysBody3D*> hammers_axis;
	p2List<PhysBody3D*> hammers;
	PhysBody3D* Hammer;
	PhysBody3D* Sstart;
	PhysBody3D* Sfinish;
	Cube startcube;
	Cube finishcube;
	int laps = 0;
	float last_lap_time = 0;
	float best_lap_time = 0;
	float current_lap_time = 0;
	float remaining_time = 0;
	Timer lap_timer;
	Timer lose_timer;
	Timer wonlost_timer;
	bool started = false;
};
