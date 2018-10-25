#include "Quadtree.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"


// Quadtree

Quadtree::Quadtree(AABB limits, int max_splits, int bucket_size): max_splits(max_splits), bucket_size(bucket_size) {
	Create(limits);
}


Quadtree::Quadtree(std::list<GameObject*> objects) { // Adaptive (not for now)

}


Quadtree::~Quadtree() {
	
	delete root; // Make sure to delete childs if any when had
}


void Quadtree::Create(AABB limits) {
	root = new QuadTreeNode(limits);
}

void Quadtree::Create(std::list<GameObject*> objects) { // Adaptive (not for now)
}


bool Quadtree::Insert(GameObject* object) {
	return root->AddObject(object, bucket_size);
}


void Quadtree::Fill(std::list<GameObject*> objects) {
	for (auto it = objects.begin(); it != objects.end(); it++)
		Insert(*it);
}
//
//template<class PRIMITIVE>
//void Quadtree::Intersect(std::list<GameObject*>& objects, PRIMITIVE primitive) {
//
//}


void Quadtree::DebugDraw() {
	root->Draw();
}

// Quadtree node
QuadTreeNode::QuadTreeNode(AABB limits) {
	box = limits;
	is_leaf = true;
	for (int i = 0; i < 4; i++)
		childs[i] = nullptr;
}

//template<class PRIMITIVE>
//void QuadTreeNode<PRIMITIVE>::CollectIntersections(std::list<GameObject*>& objects, const PRIMITIVE& primitive) {
//
//
//}


void QuadTreeNode::Split() {

	// Top left
	float quarter_x = box.HalfSize().x / 2;
	float quarter_y = box.HalfSize().z / 2;
	float3 centre = box.CenterPoint();


	AABB split_box;
	float3 new_centre;
	float3 new_box_size = float3(box.HalfSize().x, box.HalfSize().y * 2, box.HalfSize().z);

	// Top left
	new_centre = box.Centroid() + float3(-quarter_x, 0, quarter_y);
	split_box.SetFromCenterAndSize(new_centre, new_box_size);
	childs[0] = new QuadTreeNode(split_box);

	// Top right
	new_centre = box.Centroid() + float3(quarter_x, 0, quarter_y);
	split_box.SetFromCenterAndSize(new_centre, new_box_size);
	childs[1] = new QuadTreeNode(split_box);

	//Bottom left
	new_centre = box.Centroid() + float3(-quarter_x, 0, -quarter_y);
	split_box.SetFromCenterAndSize(new_centre, new_box_size);
	childs[2] = new QuadTreeNode(split_box);

	//Bottom right
	new_centre = box.Centroid() + float3(quarter_x, 0, -quarter_y);
	split_box.SetFromCenterAndSize(new_centre, new_box_size);
	childs[3] = new QuadTreeNode(split_box);
}

bool QuadTreeNode::AddObject(GameObject * obj, int bucket_size) {

	// If the gameobject is not in this node we do nothing
	if (!box.Contains(obj->getCentroid()))
		return false;

	if (objects.size() < bucket_size) // WARNING: Should be checking if "is_leaf" before trying to add anything
		objects.push_back(obj);				// If the bucket size accepts another object we add it
	else {
		if (is_leaf) {							// If it has no children, create and make it not be a leaf
			is_leaf = false;
			Split();			
		}

		for (auto it = objects.begin(); it != objects.end(); it++) 
			for (int i = 0; i < 4; i++)
				childs[i]->AddObject(*it, bucket_size);				// We first fill the childs with the objects that this node contained
		objects.clear();											// And clean the list
		
		for (int i = 0; i < 4; i++)									// We fill childs with the original object we wanted to add.
			childs[i]->AddObject(obj, bucket_size);					// If not possible, the process will repeat again until it is.
		
		// TODO 1: Make that when the maximum number of splits is reach, the bucket size stop mattering

		// TODO 2: This function could be optimized ignoring the nodes which can contain the gameobject before calling the function
	}
	return true;

}

int QuadTreeNode::getNumObj() {
	return objects.size();
}

void QuadTreeNode::Draw() {
	App->renderer3D->DrawDirectAABB(box);

	for (int i = 0; i < 4; i++)
		if (childs[i]) childs[i]->Draw();
	
}
