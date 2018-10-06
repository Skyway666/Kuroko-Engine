#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "Applog.h"
#include "GameObject.h"
#include "ComponentTransform.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(1.0f, 1.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	LookAt(Reference);

	name = "camera";
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Init(JSON_Object* config)
{
	app_log->AddLog("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	app_log->AddLog("Cleaning camera");
	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Not allow camera to be modified if UI is being operated
	if (ImGui::IsMouseHoveringAnyWindow())
		return UPDATE_CONTINUE;

	vec3 newPos(0, 0, 0);
	float speed = 1.0f * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * dt;

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) newPos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT) newPos.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (GameObject* selected_obj = App->scene_intro->selected_obj)
		{
			Vector3f centroid = Vector3f::Zero; Vector3f half_size = Vector3f::Zero;
			selected_obj->getInheritedHalfsizeAndCentroid(half_size, centroid);
			float3 new_pos = (centroid + (half_size * 1.5f)).toMathVec();
			new_pos = Quat::RotateY(((ComponentTransform*)selected_obj->getComponent(TRANSFORM))->getRotationEuler().y) * new_pos;
			Move(vec3(new_pos.x, new_pos.y, new_pos.z) - Position);
			LookAt(vec3(centroid.x, centroid.y, centroid.z));
		}
		else
			LookAt(vec3(0,0,0));
	}

	// Unfinished translation
	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT) {
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();


		if (App->input->GetKey(SDL_SCANCODE_LALT)) {
			if (GameObject* selected_obj = App->scene_intro->selected_obj) {
				Vector3f centroid = Vector3f::Zero; Vector3f half_size = Vector3f::Zero;
				selected_obj->getInheritedHalfsizeAndCentroid(half_size, centroid);
				LookAt(vec3(centroid.x, centroid.y, centroid.z));
			}
			else
				LookAt(vec3(0, 0, 0));
		}

		float Sensitivity = 0.01f;

		if (dx != 0) {
			float DeltaX = (float)dx * Sensitivity;
			newPos += X * DeltaX;
		}

		if (dy != 0) {
			float DeltaY = (float)dy * Sensitivity;
			newPos.y -= DeltaY;
		}

	}

	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{

		if(App->input->GetKey(SDL_SCANCODE_LALT)) {
			if (GameObject* selected_obj = App->scene_intro->selected_obj) {
				Vector3f centroid = Vector3f::Zero; Vector3f half_size = Vector3f::Zero;
				selected_obj->getInheritedHalfsizeAndCentroid(half_size, centroid);
				LookAt(vec3(centroid.x, centroid.y, centroid.z));
			}
			else
				LookAt(vec3(0, 0, 0));
		}
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		Position = Reference + Z * length(Position);
	}


	if (int mouse_z = App->input->GetMouseZ())
	{
		if (mouse_z > 0 && length(Position) > 0.5f)		Position *= 0.9f;
		else											Position *= 1.1f;
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;	
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
	
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return (float*)ViewMatrix.v;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	float4x4 test(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrix = test;
	ViewMatrixInverse = ViewMatrix.Inverted();
}
