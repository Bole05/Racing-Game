#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

// TODO 1: Create an enum to represent physics categories for collision detection
enum PhysicCategory
{
	DEFAULT =	1 << 0,
	PLANE =		1 << 1,
	CAR =		1 << 2,
	SHIP =		1 << 3,
	BIKE =		1 << 4
};

// TODO 4: Create an enum to define different physics groups
enum PhysicGroup {
	LAND = 1,
};

class PhysicEntity
{
protected:

	PhysicEntity(PhysBody* _body, Module* _listener)
		: body(_body)
		, listener(_listener)
	{
		body->listener = listener;
	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}

public:
	PhysBody* body;
	Module* listener;
};

class Box : public PhysicEntity
{
public:
	Box(ModulePhysics* physics, int _x, int _y, int width, int height, Module* _listener, Texture2D _texture, uint16 category, uint16 maskBits, int16 groupIndex = 0)
		: PhysicEntity(physics->CreateRectangle(_x, _y, width, height, category, maskBits, groupIndex), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f}, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:
	Texture2D texture;
};

// TODO 3: Set Category and Mask to each entity
// * Planes should not collide with other vehicles.
// * Bikes and cars should collide with bikes and cars.
// * Ships should collide with ships

// TODO 6: Remove Mask for bikes and cars and set the groupIndex to LAND

class Plane : public Box {
public:
	Plane(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture): Box(physics, _x, _y, 232, 121, _listener, _texture, PhysicCategory::PLANE, PhysicCategory::DEFAULT) {
		body->body->ApplyForce(b2Vec2(0.0f, -1000.f), body->body->GetWorldCenter(), true);
	}
};

class Bike : public Box {
public:
	Bike(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture) : Box(physics, _x, _y, 18, 35, _listener, _texture, PhysicCategory::BIKE, PhysicCategory::DEFAULT, PhysicGroup::LAND) {
	}
};

class Car : public Box {
public:
	Car(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture) : Box(physics, _x, _y, 26, 43, _listener, _texture, PhysicCategory::CAR, PhysicCategory::DEFAULT, PhysicGroup::LAND) {
		body->body->ApplyForce(b2Vec2(0.0f, 100.f), body->body->GetWorldCenter(), true);
	}
};

class Ship : public Box {
public:
	Ship(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture) : Box(physics, _x, _y, 215, 138, _listener, _texture, PhysicCategory::SHIP, PhysicCategory::DEFAULT | PhysicCategory::SHIP)
	{
	}
};



ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = false;
}

ModuleGame::~ModuleGame()
{}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	plane = LoadTexture("Assets/Plane.png"); 
	car = LoadTexture("Assets/Car.png");
	ship = LoadTexture("Assets/Ship.png");
	bike = LoadTexture("Assets/Bike.png");
	

	for (int i = 0; i < 6; ++i) {
		entities.push_back(new Car(App->physics, i * 100 + SCREEN_WIDTH * 0.25f, 100, this, car));
	}

	for (int i = 0; i < 2; ++i) {
		entities.push_back(new Ship(App->physics, i * 300 + SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * 0.5f, this, ship));
	}

	for (int i = 0; i < 6; ++i) {
		entities.push_back(new Bike(App->physics, i * 100 + SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.5f, this, bike));
	}

	for (int i = 0; i < 3; ++i) {
		entities.push_back(new Plane(App->physics, i * 300 + SCREEN_WIDTH * 0.25f, 600, this, plane));
	}

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{
	if(IsKeyPressed(KEY_SPACE))
	{
		ray_on = !ray_on;
		ray.x = GetMouseX();
		ray.y = GetMouseY();
	}

	// Prepare for raycast ------------------------------------------------------
	
	vec2i mouse;
	mouse.x = GetMouseX();
	mouse.y = GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	vec2f normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------


	for (PhysicEntity* entity : entities)
	{
		entity->Update();
		if (ray_on)
		{
			int hit = entity->RayHit(ray, mouse, normal);
			if (hit >= 0)
			{
				ray_hit = hit;
			}
		}
	}
	

	// ray -----------------
	if(ray_on == true)
	{
		vec2f destination((float)(mouse.x-ray.x), (float)(mouse.y-ray.y));
		destination.Normalize();
		destination *= (float)ray_hit;

		DrawLine(ray.x, ray.y, (int)(ray.x + destination.x), (int)(ray.y + destination.y), RED);

		if (normal.x != 0.0f)
		{
			DrawLine((int)(ray.x + destination.x), (int)(ray.y + destination.y), (int)(ray.x + destination.x + normal.x * 25.0f), (int)(ray.y + destination.y + normal.y * 25.0f), Color{ 100, 255, 100, 255 });
		}
	}

	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	
}
