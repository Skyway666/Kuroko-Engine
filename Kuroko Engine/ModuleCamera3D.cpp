#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Applog.h"
#include "ComponentTransform.h"
#include "Camera.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "Transform.h"
#include "Material.h"

#include "glew-2.1.0\include\GL\glew.h"

#define CAM_SPEED_CONST 2.5f
#define CAM_ROT_SPEED_CONST 0.25f

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "camera";
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Init(const JSON_Object* config)
{
	app_log->AddLog("Setting up the camera");
	selected_camera = editor_camera = new Camera(float3(-2.0, 2.0f, -5.0f), float3::zero);

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
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
	if (selected_camera && (!ImGui::IsMouseHoveringAnyWindow() || (ImGui::IsMouseHoveringAnyWindow() && selected_camera != editor_camera)))
	{
		// Movement
		float3 newPos = float3::zero;
		float speed = CAM_SPEED_CONST * dt;
		bool orbit = (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && selected_camera == editor_camera);

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed = 2.0f * speed;

		if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) { newPos.y += speed; };
		if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT) { newPos.y -= speed; };

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) { newPos += selected_camera->Z * speed; };
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) { newPos -= selected_camera->Z * speed; };

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) { newPos -= selected_camera->X * speed; };
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) { newPos += selected_camera->X * speed; };

		// panning
		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT && !orbit)
		{
			int dx = App->input->GetMouseXMotion();
			int dy = App->input->GetMouseYMotion();

			if (dx)		newPos += selected_camera->X * dx * speed;
			if (dy)		newPos += selected_camera->Y * dy * speed;
		}

		selected_camera->Move(newPos);

		// Rotation / Orbit

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT || (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && orbit))
		{
			int dx = App->input->GetMouseXMotion();
			int dy = App->input->GetMouseYMotion();

			float module = module = (selected_camera->Reference - selected_camera->getFrustum()->pos).Length();
			float3 X = selected_camera->X; float3 Y = selected_camera->Y; float3 Z = selected_camera->Z;

			if(orbit)
				selected_camera->LookAtSelectedGeometry();

			if (dx)
			{
				X = Quat::RotateY(dx * CAM_ROT_SPEED_CONST * DEGTORAD) * X;
				Y = Quat::RotateY(dx * CAM_ROT_SPEED_CONST * DEGTORAD) * Y;
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
				selected_camera->X = X; selected_camera->Y = Y; selected_camera->Z = Z;
			}

			if (!orbit)
				selected_camera->Reference = selected_camera->getFrustum()->pos + selected_camera->Z * module;
			else
				selected_camera->getFrustum()->pos = selected_camera->Reference - selected_camera->Z * module;
		}

		// Zooming

		if (int mouse_z = App->input->GetMouseZ())
		{
			if (mouse_z > 0)
			{
				if ((selected_camera->Reference - selected_camera->getFrustum()->pos).Length() > 1.0f)
					selected_camera->getFrustum()->pos += selected_camera->Z * (0.3f + ((selected_camera->Reference - selected_camera->getFrustum()->pos).Length() / 20));
			}
			else
				selected_camera->getFrustum()->pos -= selected_camera->Z * (0.3f + ((selected_camera->Reference - selected_camera->getFrustum()->pos).Length() / 20));
		}

		// Focus 
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && !selected_camera->getParent())
			selected_camera->FitToSizeSelectedGeometry();

		// Recalculate matrix -------------
		selected_camera->updateFrustum();

		if (selected_camera->getParent())
		{
			ComponentTransform* transform = (ComponentTransform*)selected_camera->getParent()->getParent()->getComponent(TRANSFORM);

			transform->local->setPosition(selected_camera->getFrustum()->pos);
			transform->local->setRotation(Quat::LookAt(transform->local->Forward(), selected_camera->getFrustum()->front, transform->local->Up(), float3::unitY) * transform->local->getRotation());
			transform->local->CalculateMatrix();
			transform->LocalToGlobal();
		}

	}

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size_x, size_y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//set depth renderer
	glGenRenderbuffers(1, &fb->depth_test_id);
	glBindRenderbuffer(GL_RENDERBUFFER, fb->depth_test_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size_x, size_y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb->depth_test_id);

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

