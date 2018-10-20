#include "Camera.h"
#include "GameObject.h"
#include "MathGeoLib\Math\Quat.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ModuleScene.h"

Camera::Camera(float4x4 projection_matrix, float3 position, float3 reference)
{
	ProjectionMatrix = projection_matrix;
	Position = { position.x, position.y, position.z };
	LookAt(vec3(reference.x, reference.y, reference.z));
}


// -----------------------------------------------------------------
void Camera::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void Camera::LookAt(const vec3 &Spot)
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