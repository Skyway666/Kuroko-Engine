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
	
}


ComponentRectTransform::~ComponentRectTransform()
{
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
		glLineWidth(1.5f);
		glColor3f(1.0f, 0.0f, 0.0f); // red

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draws quad unfilled
		glBegin(GL_QUADS);

		glVertex3f(rect.global.x, rect.global.y, 0);
		glVertex3f(rect.global.x + rect.width, rect.global.y, 0);
		glVertex3f(rect.global.x + rect.width, rect.global.y + rect.height, 0);
		glVertex3f(rect.global.x, rect.global.y + rect.height, 0);

		glEnd();
		App->scene->global_wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // returns back to previous polygon fill mode

		glColor3f(1.0f, 1.0f, 1.0f);
		glLineWidth(1.0f);
	}
}

void ComponentRectTransform::Save(JSON_Object * config)
{
}

inline void ComponentRectTransform::setLocalPos(float2 pos)
{
	rect.local = pos;
}

void ComponentRectTransform::setGlobalPos(float2 pos)
{
	rect.global = pos;
	if (getParent()->getParent() != nullptr) {
		ComponentRectTransform* parentTrans = (ComponentRectTransform*)getParent()->getParent()->getComponent(RECTTRANSFORM);
		float2 parentGlobal = parentTrans->getGlobalPos();
		setLocalPos(getGlobalPos() - parentGlobal);
	}
}

inline void ComponentRectTransform::setWidth(float width)
{
	rect.width = width;
}

inline void ComponentRectTransform::setHeight(float height)
{
	rect.height = height;
}
