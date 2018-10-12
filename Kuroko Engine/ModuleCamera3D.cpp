#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Applog.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleRenderer3D.h"


ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "camera";
	editor_camera = new Camera(CreatePerspMat());
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Init(const JSON_Object& config)
{
	app_log->AddLog("Setting up the camera");

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	if (editor_camera) delete editor_camera;

	for (auto it = game_cameras.begin(); it != game_cameras.end(); it++)
		delete *it;

	game_cameras.clear();
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


	bool fps = false;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		speed = 8.0f * dt;
		fps = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) {newPos.y += speed; fps = true;}
	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT) {newPos.y -= speed; fps = true;}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {newPos -= editor_camera->Z * speed; fps = true;}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {newPos += editor_camera->Z * speed; fps = true; }


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {newPos -= editor_camera->X * speed; fps = true; }
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {newPos += editor_camera->X * speed; fps = true; }

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		editor_camera->FocusSelectedGeometry();

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) 
		editor_camera->RotateSelectedGeometry();
	
		

	// Unfinished translation
	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT) {
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.01f;

		if (dx != 0) {
			float DeltaX = (float)dx * Sensitivity;
			newPos += editor_camera->X * DeltaX;
		}

		if (dy != 0) {
			float DeltaY = (float)dy * Sensitivity;
			newPos.y -= DeltaY;
		}
	}


	// Mouse motion ----------------

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		//float module = 0;
		//if(!fps)
			editor_camera->Position -= editor_camera->Reference;
		//else
		//	module = calculateModule(editor_camera->Reference - editor_camera->Position);

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			editor_camera->X = rotate(editor_camera->X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			editor_camera->Y = rotate(editor_camera->Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			editor_camera->Z = rotate(editor_camera->Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			editor_camera->Y = rotate(editor_camera->Y, DeltaY, editor_camera->X);
			editor_camera->Z = rotate(editor_camera->Z, DeltaY, editor_camera->X);

			if (editor_camera->Y.y < 0.0f)
			{
				editor_camera->Z = vec3(0.0f, editor_camera->Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				editor_camera->Y = cross(editor_camera->Z, editor_camera->X);
			}
		}

		//if(!fps)
			editor_camera->Position = editor_camera->Reference + editor_camera->Z * length(editor_camera->Position);
		//else
		//	editor_camera->Reference = editor_camera->Position + editor_camera->Z * module;
	}


	if (int mouse_z = App->input->GetMouseZ())
	{
		if (mouse_z > 0 && length(editor_camera->Position) > 0.5f)		
			editor_camera->Position = editor_camera->Reference + editor_camera->Z * length(editor_camera->Position - editor_camera->Reference) * 0.9f;
		else															
			editor_camera->Position = editor_camera->Reference + editor_camera->Z * length(editor_camera->Position - editor_camera->Reference) *1.1f;
	}

	// Recalculate matrix -------------
	editor_camera->Move(newPos);

	return UPDATE_CONTINUE;	
}


float4x4 ModuleCamera3D::CreatePerspMat(float fov, float width, float height, float near_plane, float far_plane)
{
	float4x4 Perspective = float4x4::zero;
	float aspect_ratio = width / height;

	float coty = 1.0f / tan(fov * (float)M_PI / 360.0f);

	Perspective.v[0][0] = coty / aspect_ratio;
	Perspective.v[1][1] = coty;
	Perspective.v[2][2] = (near_plane + far_plane) / (near_plane - far_plane);
	Perspective.v[2][3] = -1.0f;
	Perspective.v[3][2] = 2.0f * near_plane * far_plane / (near_plane - far_plane);
	Perspective.v[3][3] = 0.0f;

	return Perspective;
}

float ModuleCamera3D::calculateModule(vec3 vec) {

	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

Camera::Camera(float4x4 projection_matrix, float3 position, float3 reference)
{
	ProjectionMatrix = projection_matrix;
	Position = { position.x, position.y, position.z };
	LookAt(vec3(reference.x, reference.y, reference.z ));
}

Camera::~Camera()
{
	App->renderer3D->frame_buffers_to_delete.push_back(frame_buffer);
}

// -----------------------------------------------------------------
void Camera::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
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
void Camera::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
	
}


// -----------------------------------------------------------------
void Camera::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* Camera::GetViewMatrix() const
{
	return (float*)ViewMatrix.v;
}

// -----------------------------------------------------------------
void Camera::CalculateViewMatrix()
{
	float4x4 matrix(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrix = matrix;
}

void Camera::RotateSelectedGeometry() {

	if (GameObject* selected_obj = App->scene->selected_obj) {
		float3 centroid = float3::zero; float3 half_size = float3::zero;
		selected_obj->getInheritedHalfsizeAndCentroid(half_size, centroid);
		LookAt(vec3(centroid.x, centroid.y, centroid.z));
	}
	else
		LookAt(vec3(0, 0, 0));

}

void Camera::FocusSelectedGeometry(float distance) {
	if (GameObject* selected_obj = App->scene->selected_obj) {
		float3 centroid = float3::zero; float3 half_size = float3::zero;
		selected_obj->getInheritedHalfsizeAndCentroid(half_size, centroid);
		float3 new_pos = centroid + half_size + float3(distance, distance, distance);
		new_pos = Quat::RotateY(((ComponentTransform*)selected_obj->getComponent(TRANSFORM))->getRotationEuler().y) * new_pos;
		Move(vec3(new_pos.x, new_pos.y, new_pos.z) - Position);
		LookAt(vec3(centroid.x, centroid.y, centroid.z));
	}
	else
		LookAt(vec3(0, 0, 0));
}

void Camera::Reset() {
	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(1.0f, 1.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	LookAt(Reference);
}