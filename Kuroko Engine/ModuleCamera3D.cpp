#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Applog.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleRenderer3D.h"

#define CAM_SPEED_CONST 1.0f
#define CAM_ROT_SPEED_CONST 0.25f

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

	// Movement

	vec3 newPos(0, 0, 0);
	float speed = CAM_SPEED_CONST * dt;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) 
		speed = 2.0f * speed;
	
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) { newPos.y += speed; };
	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT) { newPos.y -= speed; };

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) { newPos -= editor_camera->Z * speed; };
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) { newPos += editor_camera->Z * speed; };

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) { newPos -= editor_camera->X * speed; };
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) { newPos += editor_camera->X * speed; };

	// panning
	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT) 
	{
		int dx = App->input->GetMouseXMotion();
		int dy = App->input->GetMouseYMotion();

		if (dx)		newPos -= editor_camera->X * dx * speed;
		if (dy)		newPos += editor_camera->Y * dy * speed;
	}

	editor_camera->Move(newPos);

	// Rotation / Orbit

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT || (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT))
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float module = 0.0f;

		if (App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
			module = length(editor_camera->Reference - editor_camera->Position);
		else
		{
			editor_camera->LookAtSelectedGeometry();
			editor_camera->Position -= editor_camera->Reference;
		}

		if (dx)
		{
			editor_camera->X = rotate(editor_camera->X, dx * CAM_ROT_SPEED_CONST, vec3(0.0f, 1.0f, 0.0f));
			editor_camera->Y = rotate(editor_camera->Y, dx * CAM_ROT_SPEED_CONST, vec3(0.0f, 1.0f, 0.0f));
			editor_camera->Z = rotate(editor_camera->Z, dx * CAM_ROT_SPEED_CONST, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy)
		{
			editor_camera->Y = rotate(editor_camera->Y, dy * CAM_ROT_SPEED_CONST, editor_camera->X);
			editor_camera->Z = rotate(editor_camera->Z, dy * CAM_ROT_SPEED_CONST, editor_camera->X);

			if (editor_camera->Y.y < 0.0f)
			{
				editor_camera->Z = vec3(0.0f, editor_camera->Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				editor_camera->Y = cross(editor_camera->Z, editor_camera->X);
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
			editor_camera->Reference = editor_camera->Position + editor_camera->Z * module;
		else
			editor_camera->Position = editor_camera->Reference + editor_camera->Z * length(editor_camera->Position);
	}

	// Zooming

	if (int mouse_z = App->input->GetMouseZ())
	{
		if (mouse_z > 0 && length(editor_camera->Position) > 0.5f)		
			editor_camera->Position = editor_camera->Reference + editor_camera->Z * length(editor_camera->Position - editor_camera->Reference) * 0.9f;
		else															
			editor_camera->Position = editor_camera->Reference + editor_camera->Z * length(editor_camera->Position - editor_camera->Reference) *1.1f;
	}
	
	// Focus 
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		editor_camera->FitToSizeSelectedGeometry();

	// Recalculate matrix -------------
	editor_camera->CalculateViewMatrix();

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

void Camera::LookAtSelectedGeometry()
{
	if (GameObject* selected_obj = App->scene->selected_obj) 
	{
		float3 centroid = float3::zero;
		selected_obj->getInheritedHalfsizeAndCentroid(float3(), centroid);
		LookAt(vec3(centroid.x, centroid.y, centroid.z));
	}
	else
		LookAt(vec3(0, 0, 0));

}

void Camera::FitToSizeSelectedGeometry(float distance)
{
	if (GameObject* selected_obj = App->scene->selected_obj) 
	{
		float3 centroid = float3::zero; float3 half_size = float3::zero;
		selected_obj->getInheritedHalfsizeAndCentroid(half_size, centroid);

		float3 new_pos = centroid + half_size + float3(distance, distance, distance);
		new_pos = Quat::RotateY(((ComponentTransform*)selected_obj->getComponent(TRANSFORM))->getRotationEuler().y) * new_pos;

		Position = { new_pos.x, new_pos.y, new_pos.z };
		LookAt(vec3(centroid.x, centroid.y, centroid.z));
	}
	else
		LookAt(vec3(0, 0, 0));
}

void Camera::Reset() 
{
	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(1.0f, 1.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	LookAt(Reference);
}