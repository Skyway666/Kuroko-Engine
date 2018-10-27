#ifndef QUAD_TREE
#define QUAD_TREE
#include <list>
#include "Math.h"

class GameObject;

struct QuadTreeNode {

	QuadTreeNode(AABB limits); // Create nodes with no objectsç
	~QuadTreeNode();
	void Split(); // creates childs and passes them the objects contained on this one
	bool AddObject(GameObject* obj, int bucket_size);
	int getNumObj();
	void Draw();
	QuadTreeNode* childs[4];
	AABB box;
	bool is_leaf = true;
	std::list<GameObject*> objects;

/*	template<class PRIMITIVE>
	void CollectIntersections(std::list<GameObject*>& found_obj, const PRIMITIVE& primitive); */// See what intersections the nodes have
};




class Quadtree {
public:
	Quadtree(AABB limits, int max_splits = 8, int bucket_size = 1);
	Quadtree(std::list<GameObject*> objects);	  // Adaptive(not for now)
	~Quadtree();
	void Create(AABB limits); 	
	void Create(std::list<GameObject*> objects);	  // Adaptive(not for now)
	bool Insert(GameObject* object);			  // Insert an object into the quadtree so it blends to fit it
	void Fill(std::list<GameObject*> objects);    //Multiple "Inserts" at once
	void CleanUp();
	void Empty();
	void DebugDraw();
	QuadTreeNode* root; // Root of the quadtree
	int max_splits; 	// How many splits allowed (not used for now)
	int bucket_size;	// How much items can be held in a QuadTreeNode
/*
	template<class PRIMITIVE>
	void Intersect(std::list<GameObject*>& objects, PRIMITIVE primitive); */	// objects: List to be filled
};





#endif 
