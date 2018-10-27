#include "Camera.h"
#include "GameObject.h"
#include "MathGeoLib\Math\Quat.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ModuleScene.h"

Camera::Camera(float4x4 projection_matrix, float3 position, float3 reference)
{
	ProjectionMatrix = projection_matrix;
	Position = position;
	LookAt(reference);
}


// -----------------------------------------------------------------
void Camera::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = float3::unitY.Cross(Z).Normalized();
	Y = Z.Cross(X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void Camera::LookAt(const float3 &Spot)
{
	Reference = Spot;

	Z = (Position - Reference).Normalized();
	X = float3::unitY.Cross(Z).Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();

}


// -----------------------------------------------------------------
void Camera::Move(const float3 &Movement)
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
	float4x4 matrix(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -Position.Dot(X), -Position.Dot(Y), -Position.Dot(Z), 1.0f);
	ViewMatrix = matrix;
}

void Camera::LookAtSelectedGeometry()
{
	if (GameObject* selected_obj = App->scene->selected_obj)
	{
		float3 centroid = float3::zero;
		selected_obj->getInheritedHalfsizeAndCentroid(float3(), centroid);
		LookAt(centroid);
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
		LookAt(centroid);
	}
	else
		LookAt(float3::zero);
}

void Camera::Reset()
{
	Position = float3(1.0f, 1.0f, 5.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);

	Z = (Position - Reference).Normalized();
	X = float3::unitY.Cross(Z).Normalized();
	Y = Z.Cross(X);

	LookAt(Reference);
}