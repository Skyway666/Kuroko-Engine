#include "Transform.h"
#include "MathGeoLib\Math\TransformOps.h"
#include "ImGui\ImGuizmo.h"
#include "ImGui\imgui.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "Camera.h"
#include "glew-2.1.0\include\GL\glew.h"

//void EditTransform(const float *cameraView, float *cameraProjection, float* matrix) {
//	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
//	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
//	static bool useSnap = false;
//	static float snap[3] = { 1.f, 1.f, 1.f };
//	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
//	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
//	static bool boundSizing = false;
//	static bool boundSizingSnap = false;
//
//	if (ImGui::IsKeyPressed(90))
//		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
//	if (ImGui::IsKeyPressed(69))
//		mCurrentGizmoOperation = ImGuizmo::ROTATE;
//	if (ImGui::IsKeyPressed(82)) // r Key
//		mCurrentGizmoOperation = ImGuizmo::SCALE;
//	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
//		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
//	ImGui::SameLine();
//	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
//		mCurrentGizmoOperation = ImGuizmo::ROTATE;
//	ImGui::SameLine();
//	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
//		mCurrentGizmoOperation = ImGuizmo::SCALE;
//	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
//	ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
//	ImGui::InputFloat3("Tr", matrixTranslation, 3);
//	ImGui::InputFloat3("Rt", matrixRotation, 3);
//	ImGui::InputFloat3("Sc", matrixScale, 3);
//	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);
//
//	if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
//		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
//			mCurrentGizmoMode = ImGuizmo::LOCAL;
//		ImGui::SameLine();
//		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
//			mCurrentGizmoMode = ImGuizmo::WORLD;
//	}
//	if (ImGui::IsKeyPressed(83))
//		useSnap = !useSnap;
//	ImGui::Checkbox("", &useSnap);
//	ImGui::SameLine();
//
//	switch (mCurrentGizmoOperation) {
//	case ImGuizmo::TRANSLATE:
//		ImGui::InputFloat3("Snap", &snap[0]);
//		break;
//	case ImGuizmo::ROTATE:
//		ImGui::InputFloat("Angle Snap", &snap[0]);
//		break;
//	case ImGuizmo::SCALE:
//		ImGui::InputFloat("Scale Snap", &snap[0]);
//		break;
//	}
//	ImGui::Checkbox("Bound Sizing", &boundSizing);
//	if (boundSizing) {
//		ImGui::PushID(3);
//		ImGui::Checkbox("", &boundSizingSnap);
//		ImGui::SameLine();
//		ImGui::InputFloat3("Snap", boundsSnap);
//		ImGui::PopID();
//	}
//
//	ImGuiIO& io = ImGui::GetIO();
//	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
//	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
//}

Transform::Transform(const Transform& transform)
{
	setPosition(transform.position);
	setRotation(transform.rotation);
	setScale(transform.scale);
	CalculateMatrix();
}

Transform::Transform(JSON_Object* deff) {

	setPosition(float3(json_object_get_number(deff, "px"), json_object_get_number(deff, "py"), json_object_get_number(deff, "pz")));
	setScale(float3(json_object_get_number(deff, "sx"), json_object_get_number(deff, "sy"), json_object_get_number(deff, "sz")));
	Quat rotation;
	rotation.x = json_object_get_number(deff, "qx");
	rotation.y = json_object_get_number(deff, "qy");
	rotation.z = json_object_get_number(deff, "qz");
	rotation.w = json_object_get_number(deff, "qw");
	setRotation(rotation);
}

void Transform::setRotationEuler(const float3& euler)
{
	rotation = Quat::identity;
	rotation = rotation * rotation.RotateX(DEGTORAD * (euler.x));
	rotation = rotation * rotation.RotateY(DEGTORAD * (euler.y));
	rotation = rotation * rotation.RotateZ(DEGTORAD * (euler.z));
	euler_angles = euler;
}

void Transform::LookAt(const float3& position, const float3& target, const float3& forward, const float3& up)
{
	mat = mat.LookAt(position, target, forward, up, float3::unitY);
	rotation = mat.RotatePart().ToQuat();
}

void Transform::DrawGuizmo() {

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

	ImGuizmo::BeginFrame();
	float4x4 projection4x4 = App->camera->editor_camera->getFrustum()->ProjectionMatrix();
	float4x4 view4x4 = App->camera->editor_camera->getViewMatrix();


	//glGetFloatv(GL_MODELVIEW_MATRIX, (float*)view4x4.v);
	//glGetFloatv(GL_PROJECTION_MATRIX, (float*)projection4x4.v);

	//view4x4 = float4x4::identity;

	CalculateMatrix();


	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetOrthographic(true);
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	//ImGuizmo::DrawCube((float*)view4x4.v, (float*)projection4x4.v, new_matrix);
	ImGuizmo::Manipulate((float*)view4x4.v, (float*)projection4x4.v, mCurrentGizmoOperation, mCurrentGizmoMode, (float*)mat.v, NULL, NULL);

	//ApplyMatrix((float*)mat.v);
}

float4x4 Transform::CalculateMatrix()
{

	// WAY 1
	//mat = float4x4::identity;
	//mat = mat * rotation;
	//mat = mat * mat.Scale(scale);
	//mat.SetTranslatePart(position);

	// WAY 2
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];

	matrixTranslation[0] = position.x;
	matrixTranslation[1] = position.y;
	matrixTranslation[2] = position.z;

	matrixRotation[0] = euler_angles.x;
	matrixRotation[1] = euler_angles.y;
	matrixRotation[2] = euler_angles.z;

	matrixScale[0] = scale.x;
	matrixScale[1] = scale.y;
	matrixScale[2] = scale.z;

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (float*)mat.v); // When recomposing the matrix from the components, it is transposed
	mat.Transpose(); 
	return mat;
}

void Transform::ApplyMatrix(float * m) {

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(m, matrixTranslation, matrixRotation, matrixScale);

	position.x = matrixTranslation[0];
	position.y = matrixTranslation[1];
	position.z = matrixTranslation[2];

	setRotationEuler(float3(matrixRotation[0], matrixRotation[1], matrixRotation[2]));

	scale.x = matrixScale[0];
	scale.y = matrixScale[1];
	scale.z = matrixScale[2];
}

float3 Transform::Right() const
{
	float3 right = float3::unitX;
	right = rotation * right;
	return right;
}

float3 Transform::Forward() const
{
	float3 forward = float3::unitZ;
	forward = rotation * forward;
	return forward;
}

float3 Transform::Up() const
{
	float3 up = float3::unitY;
	up = rotation * up;
	return up;
}

void Transform::setPosition(const float3& pos) { position = pos; };
void Transform::Translate(const float3& dir) { position += dir; };
void Transform::setScale(const float3& scl) { scale = scl; };
void Transform::Scale(const float3& scl) { scale.x *= scl.x; scale.y *= scl.y; scale.z *= scl.z; };