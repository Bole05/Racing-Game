#pragma once

#include "Module.h"
#include "Globals.h"
#include "pugixml.hpp"
#include "box2d\box2d.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y 0.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum BodyType {
	UNKNOWN = 0,
	PLAYER,
	ENEMY,
	WALL,
	BOOST
};



struct VehicleInfo {
	int width = 26;
	int height = 43;
	float linearDamping = 0.2f;
	float angularDamping = 2.0f;

	// Estadísticas de conducción
	float acceleration = 2.0f;    // La "speed" del player
	float maxSpeed = 10.0f;
	float turnSpeed = 3.0f;       // Velocidad de giro
	float turnDrag = 0.96f;       // Fricción al girar
	float friction = 0.98f;       // Fricción natural del suelo
};
// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL),width(0),height(0),ptype(UNKNOWN)
	{}

	//void GetPosition(int& x, int& y) const;
	void GetPhysicPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:	
	int width, height;
	b2Body* body;
	Module* listener;
	BodyType ptype;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	update_status Update();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius);
	PhysBody* CreateRectangle(int x, int y, int width, int height, uint16 categoryBits, uint16 maskBits, int16 groupIndex = 0);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height);
	PhysBody* CreateChain(int x, int y, const int* points, int size);
	PhysBody* CreateRacingCar(int x, int y, const VehicleInfo& info);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	bool debug=false;

private:

	/*bool debug;*/
	b2World* world;
	b2MouseJoint* mouse_joint;
	b2Body* ground;
};