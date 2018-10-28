#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Applog.h"
#include "ComponentTransform.h"
#include "Camera.h"
#include "Material.h"

#include "glew-2.1.0\include\GL\glew.h"

#define CAM_SPEED_CONST 2.5f
#define CAM_ROT_SPEED_CONST 0.25f

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "camera";
	editor_camera = new Camera(float3(0.0, 2.0f, 5.0f), float3::zero);
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
	Camera aux = *editor_camera;

	float3 newPos = float3::zero;
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
		float3 X = editor_camera->X; float3 Y = editor_camera->Y; float3 Z = editor_camera->Z;

		if (App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
			module = (editor_camera->Reference - editor_camera->getFrustum()->pos).Length();
		else
		{
			editor_camera->LookAtSelectedGeometry();
			editor_camera->getFrustum()->pos -= editor_camera->Reference;
		}

		if (dx)
		{
			X = Quat::RotateY(dx * CAM_ROT_SPEED_CONST * DEGTORAD) * X;
			Y = Quat::RotateY( dx * CAM_ROT_SPEED_CONST * DEGTORAD) * Y;
			Z = Quat::RotateY(dx * CAM_ROT_SPEED_CONST * DEGTORAD) * Z;
		}

		if (dy)
		{
			Y = Quat::RotateAxisAngle(X, dy * CAM_ROT_SPEED_CONST * DEGTORAD) * Y;
			Z = Quat::RotateAxisAngle(X, dy * CAM_ROT_SPEED_CONST * DEGTORAD) * Z;
			
			if (Y.y < 0.0f)
			{
				Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = Z.Cross(X);
			}
		}
		
		if (!IsNan(-X.x) && !IsNan(-Y.x) && !IsNan(-Z.x))
		{
			editor_camera->X = X; editor_camera->Y = Y; editor_camera->Z = Z;
		}

		if (App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
			editor_camera->Reference = editor_camera->getFrustum()->pos - editor_camera->Z * module;
		else
			editor_camera->getFrustum()->pos = editor_camera->Reference + editor_camera->Z * editor_camera->getFrustum()->pos.Length();
	}

	// Zooming

	if (int mouse_z = App->input->GetMouseZ())
	{
		if (mouse_z > 0)
		{
			if((editor_camera->Reference - editor_camera->getFrustum()->pos).Length() > 1.0f)
				editor_camera->getFrustum()->pos -= editor_camera->Z * (0.3f + ((editor_camera->Reference - editor_camera->getFrustum()->pos).Length() / 20));
		}
		else															
			editor_camera->getFrustum()->pos += editor_camera->Z * (0.3f + ((editor_camera->Reference - editor_camera->getFrustum()->pos).Length() / 20));
	}
	
	// Focus 
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		editor_camera->FitToSizeSelectedGeometry();

	// Recalculate matrix -------------
	editor_camera->updateFrustum();

	return UPDATE_CONTINUE;	
}


FrameBuffer* ModuleCamera3D::initFrameBuffer(uint size_x, uint size_y)
{
	FrameBuffer* fb = new FrameBuffer();
	fb->size_x = size_x; fb->size_y = size_y;
	// set frame buffer
	glGenFramebuffers(1, &fb->id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->id);

	// set frame buffer texture
	fb->tex = new Texture();
	glGenTextures(1, &fb->tex->gl_id);
	glBindTexture(GL_TEXTURE_2D, fb->tex->gl_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// set depth texture
	fb->depth_tex = new Texture();
	glGenTextures(1, &fb->depth_tex->gl_id);
	glBindTexture(GL_TEXTURE_2D, fb->depth_tex->gl_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//set depth renderer
	glGenRenderbuffers(1, &fb->depth_tex->gl_id);
	glBindRenderbuffer(GL_RENDERBUFFER, fb->depth_tex->gl_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size_x, size_y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb->depth_tex->gl_id);

	// init frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->tex->gl_id, 0);
	glFramebufferTexture2D(GL_DEPTH_ATTACHMENT, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->depth_tex->gl_id, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	frame_buffers.push_back(fb);
	return fb;
}

