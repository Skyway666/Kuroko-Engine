#include "Quadtree.h"
#include "GameObject.h"


// Quadtree
template<typename PRIMITIVE>
Quadtree<PRIMITIVE>::Quadtree(AABB limits, int max_splits, int bucket_size): max_splits(max_splits), bucket_size(bucket_size) {
	Create(limits);
}

template<typename PRIMITIVE> // Adaptive (not for now)
Quadtree<PRIMITIVE>::Quadtree(std::list<GameObject*> objects) {

}

template<class PRIMITIVE>
Quadtree<PRIMITIVE>::~Quadtree() {
	delete root; // Make sure to delete childs if any when had
}

template<class PRIMITIVE>
void Quadtree<PRIMITIVE>::Create(AABB limits) {
	root = new QuadTreeNode<PRIMITIVE>(limits);
}

template<class PRIMITIVE> // Adaptive (not for now)
void Quadtree<PRIMITIVE>::Create(std::list<GameObject*> objects) {
}

template<class PRIMITIVE>
bool Quadtree<PRIMITIVE>::Insert(GameObject* object) {
	return root->AddObject(object);
}

template<class PRIMITIVE>
void Quadtree<PRIMITIVE>::Fill(std::list<GameObject*> objects) {
	for (auto it = objects.begin(); it != objects.end(); it++)
		Insert(*it);
}

template<class PRIMITIVE>
void Quadtree<PRIMITIVE>::Intersect(std::list<GameObject*>& objects, PRIMITIVE primitive) {

}

template<class PRIMITIVE>
void Quadtree<PRIMITIVE>::DebugDraw() {

}

// Quadtree node
template<class PRIMITIVE>
QuadTreeNode<PRIMITIVE>::QuadTreeNode(AABB limits) {
	box = limits;
	is_leaf = true;
}


template<class PRIMITIVE> // Constructor to create childs
QuadTreeNode<PRIMITIVE>::QuadTreeNode(AABB limits, std::list<GameObject*> objects) {


}

template<class PRIMITIVE>
void QuadTreeNode<PRIMITIVE>::CollectIntersections(std::list<GameObject*>& objects, const PRIMITIVE& primitive) {


}


template<class PRIMITIVE>
void QuadTreeNode<PRIMITIVE>::Split() {

	// Top left
	float quarter_x = box.HalfSize().x / 2;
	float quarter_y = box.HalfSize().z / 2;
	float3 centre = box.CenterPoint();

	
	AABB split_box;
	float3 new_centre;

	// Top left
	new_centre = box.Centroid + float3(-quarter_x, 0, quarter_y);
	split_box.SetFromCenterAndSize(new_centre, box.HalfSize());
	childs[0] = new QuadTreeNode<PRIMITIVE>(split_box);

	// Top right
	new_centre = box.Centroid + float3(quarter_x, 0, quarter_y);
	split_box.SetFromCenterAndSize(new_centre, box.HalfSize());
	childs[1] = new QuadTreeNode<PRIMITIVE>(split_box);

	//Bottom left
	new_centre = box.Centroid + float3(-quarter_x, 0, -quarter_y);
	split_box.SetFromCenterAndSize(new_centre, box.HalfSize());
	childs[2] = new QuadTreeNode<PRIMITIVE>(split_box);

	//Bottom right
	new_centre = box.Centroid + float3(quarter_x, 0, -quarter_y);
	split_box.SetFromCenterAndSize(new_centre, box.HalfSize());
	childs[3] = new QuadTreeNode<PRIMITIVE>(split_box);
}

template<class PRIMITIVE>
bool QuadTreeNode<PRIMITIVE>::AddObject(GameObject * obj, int bucket_size) {

	// If the gameobject is not in this node we do nothing
	if (!box.Contains(obj->getCentroid()))
		return false;

	if (objects.size() < bucket_size)
		objects.push_back(obj);				// If the bucket size accepts another object we add it
	else {
		is_leaf = false						
		Split();							// If not we split the node, which makes it not be a leaf anymore, 

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

template<class PRIMITIVE>
int QuadTreeNode<PRIMITIVE>::getNumObj() {
	return objects.size();
}
