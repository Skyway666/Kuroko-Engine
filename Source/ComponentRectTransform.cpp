#include "ComponentRectTransform.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "glew-2.1.0\include\GL\glew.h"


ComponentRectTransform::ComponentRectTransform(GameObject* parent) : Component(parent, RECTTRANSFORM)
{
	setWidth(1.0f);
	setHeight(1.0f);

	debug_draw = true;
	
	static const float vtx[] = {
		rect.global.x,  rect.global.y, 0,
		rect.global.x + rect.width, rect.global.y, 0,
		rect.global.x + rect.width, rect.global.y + rect.height, 0,
		rect.global.x, rect.global.y + rect.height, 0
	};
	rect.vertex = new float3[4];
	memcpy(rect.vertex, vtx, sizeof(float3) * 4);

	GenBuffer();
}


ComponentRectTransform::~ComponentRectTransform()
{
	//RELEASE MACRO NEEDED
	delete[] rect.vertex;
	rect.vertex = nullptr;
}

bool ComponentRectTransform::Update(float dt)
{
	/*
	if (getParent()->getComponent(CANVAS) != nullptr) {//it is canvas
		setGlobalPos(getLocalPos());
	}
	else {
		ComponentRectTransform* parentTrans = (ComponentRectTransform*)getParent()->getComponent(RECTTRANSFORM);
		float2 globalTrans = parentTrans->getGlobalPos() + getLocalPos();
		setLocalPos(globalTrans);
	}*/
	return true;
}

void ComponentRectTransform::Draw() const
{
	if (debug_draw) {

		glPushMatrix();
		float4x4 globalMat;
		globalMat = float4x4::FromTRS(float3(rect.global.x, rect.global.y, 0), Quat(0, 0, 0, 0), float3(rect.width, rect.height, 0));
		glMultMatrixf(globalMat.Transposed().ptr());

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, 0); //resets the buffer

		glLineWidth(1.5f);
		glColor3f(1.0f, 0.0f, 0.0f); // red
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draws quad unfilled

		glBindBuffer(GL_ARRAY_BUFFER, rect.vertexID);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glDrawArrays(GL_QUADS, 0, 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0); //resets the buffer
		glDisableClientState(GL_VERTEX_ARRAY);
		glPopMatrix();

		App->scene->global_wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // returns back to previous polygon fill mode

		glLineWidth(3.0f);

		//----Anchor Point
		glBegin(GL_LINES);
		glVertex3f(rect.anchor.x, rect.anchor.y + 0.1f, 0);
		glVertex3f(rect.anchor.x, rect.anchor.y - 0.1f, 0);
		glVertex3f(rect.anchor.x + 0.1f, rect.anchor.y, 0);
		glVertex3f(rect.anchor.x - 0.1f, rect.anchor.y, 0);
		glEnd();

		glColor3f(1.0f, 1.0f, 1.0f);
		glLineWidth(1.0f);


	}
}

void ComponentRectTransform::Save(JSON_Object * config)
{
}


void ComponentRectTransform::setPos(float2 pos)
{
	float2 dist = pos - rect.local;
	rect.local = pos;
	rect.global +=dist;

	rect.anchor.x = rect.global.x + rect.width / 2;
	rect.anchor.y = rect.global.y + rect.height / 2;

	UpdateGlobalMatrixRecursive(this);
}


void ComponentRectTransform::GenBuffer()
{
	glGenBuffers(1, (GLuint*) &(rect.vertexID));
	glBindBuffer(GL_ARRAY_BUFFER, rect.vertexID); // set the type of buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * 4, &rect.vertex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void ComponentRectTransform::UpdateGlobalMatrixRecursive(ComponentRectTransform* rect)
{
	std::list<GameObject*> childs = std::list<GameObject*>();
	rect->getParent()->getChildren(childs);	

	for (auto it = childs.begin(); it != childs.end(); it++) {
		ComponentRectTransform* childRect = (ComponentRectTransform*)(*it)->getComponent(RECTTRANSFORM);
		childRect->setGlobalPos(rect->getGlobalPos() + childRect->getLocalPos());
		UpdateGlobalMatrixRecursive(childRect);
	}
}
