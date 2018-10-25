#include "Quadtree.h"


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
void Quadtree<PRIMITIVE>::Insert(GameObject * object) {

}

template<class PRIMITIVE>
void Quadtree<PRIMITIVE>::Fill(std::list<GameObject*> objects) {
	for (auto it = objects.begin(); it != objects.end(); it++)
		Insert(*it);
}

template<class PRIMITIVE>
void Quadtree<PRIMITIVE>::Intersect(std::list<GameObject*>& objects, PRIMITIVE primitive) {

}

// Quadtree node
template<class PRIMITIVE>
QuadTreeNode<PRIMITIVE>::QuadTreeNode(AABB limits) {
	box = limits;
}


template<class PRIMITIVE>
QuadTreeNode<PRIMITIVE>::QuadTreeNode(AABB limits, std::list<GameObject*> objects) {


}

template<class PRIMITIVE>
void QuadTreeNode<PRIMITIVE>::CollectIntersections(std::list<GameObject*>& objects, const PRIMITIVE& primitive) {


}


template<class PRIMITIVE>
void QuadTreeNode<PRIMITIVE>::Split() {

}