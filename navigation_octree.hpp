#pragma once

// #include <stdint.h>

// #include "vector.h"
// #include "octant.h"
// #include "../utils/queue.h"
// #include "../utils/queue.h"

// NOTE: THIS WORKS!
namespace godot {
	class Vector3;
	class PhysicsDirectSpaceState3D;
	// class BoxShape3D;
	class PhysicsShapeQueryParameters3D;
}

static inline void bar(void);
int baz(int a, int b=22);

// struct Vector3;
struct Octant;
struct Portal;

template<typename T>
class Queue;

class NavigationOctree {

public:

	NavigationOctree();
	~NavigationOctree();

private:

	Octant *root;

public:

	static inline void foo(void);

	static void guh(void)
	{
		/* IMAGINE */
	}

	void generate(const godot::Vector3& origin, const godot::Vector3& halfSize, godot::PhysicsDirectSpaceState3D* space, const godot::Vector3& stopSize);

	bool lineOfSight(const godot::Vector3& pos0, const godot::Vector3& pos1) const;

	// to RID

	// from RID

	Octant* nearestLeaf(const godot::Vector3& pos, bool& containsPos) const;

	Octant* nearestEmptyLeaf(const godot::Vector3& pos, bool& containsPos) const;

private:

	Octant** intersectedOctants(const godot::Vector3& pos0, const godot::Vector3& pos1) const;

	bool intersectsOctant(Octant* octant, const godot::Vector3& pos0, const godot::Vector3& delta) const;

	godot::Vector3 nearestPoint(Octant* octant, const godot::Vector3& pos) const;

	void generateOctree(godot::PhysicsDirectSpaceState3D *space, const godot::Vector3& stopSize);

	void subdivide(Octant* o, godot::PhysicsShapeQueryParameters3D* query, godot::PhysicsDirectSpaceState3D* space, const godot::Vector3& stopSize);

	// mnb it optimizes raycasting?
	int mergeNonemptyLeaves(Octant* octant, Octant** children) const;

	void findNeighbors(Octant* o);

	void shallowestNeighbors(Octant* octant, const unsigned char pos0, Queue<Octant*>* res) const;

	void deepestNeighbors(Octant* octant, Octant* neighbor, Queue<Octant*>* res, Queue<unsigned char>* dirs) const;

	// void octantsFacing(Octant* octant, const uint8_t dir, const uint8_t corner, Octant** res, uint8_t& size) const;

	void leavesFacing(Octant* octant, const unsigned char dir, Queue<Octant*>* res) const;

}; // end Octree
