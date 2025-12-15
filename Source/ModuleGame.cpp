#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleMap.h"
#include "ModulePlayer.h"
#include"ModuleAi.h"
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
	LOG("Loading Intro assets---------------------------------------------------------------------");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;
	App->map->Load("Assets-racing/Maps/MapTemplate.tmx");

	//plane = LoadTexture("Assets/Plane.png"); 
	//car = LoadTexture("Assets/Car.png");
	//ship = LoadTexture("Assets/Ship.png");
	//bike = LoadTexture("Assets/Bike.png");
	//
	//for (int i = 0; i < 6; ++i) {
	//	Car* new_car = new Car(App->physics, i * 100 + SCREEN_WIDTH * 0.25f, 100, this, car);
	//	entities.push_back(new_car);
	//	if (i == 0) {
	//		car_to_track = new_car->body; // Se hace seguimiento al primer coche
	//	}
	//}

	//for (int i = 0; i < 6; ++i) {
	//	entities.push_back(new Car(App->physics, i * 100 + SCREEN_WIDTH * 0.25f, 100, this, car));
	//}

	//for (int i = 0; i < 2; ++i) {
	//	entities.push_back(new Ship(App->physics, i * 300 + SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * 0.5f, this, ship));
	//}

	//for (int i = 0; i < 6; ++i) {
	//	entities.push_back(new Bike(App->physics, i * 100 + SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.5f, this, bike));
	//}

	//for (int i = 0; i < 3; ++i) {
	//	entities.push_back(new Plane(App->physics, i * 300 + SCREEN_WIDTH * 0.25f, 600, this, plane));
	//}

	// --- CREACION DE SENSORES DE VUELTA ---
	// Posiciones estimadas para un circuito rectangular:
	// S1: Línea de inicio/meta (arriba a la izquierda)
	// S2: Arriba a la derecha
	// S3: Abajo a la derecha
	// S4: Abajo a la izquierda
	//Car* new_car = new Car(App->physics, 700, 600, this, car); // Posición cerca de S1
	//entities.push_back(new_car);
	//car_to_track = new_car->body;

	//App->player->pbody es el cuerpo físico que acabas de crear en ModulePlayer
		// Lo asignamos a la variable de seguimiento del juego.
	car_to_track = App->player->pbody;
	

	// Crear un par de enemigos en posiciones diferentes de la ruta
	App->Ai->CreateEnemy(0);  // Empieza en el punto 0
	App->Ai->CreateEnemy(5);  // Empieza en el punto 5 (más adelante)
	

	int sensor_width = 10;
	int sensor_height = 100;

	// S1 (Inicio/Fin): cerca de la posición inicial del coche, línea vertical
	sensor1 = App->physics->CreateRectangleSensor(600, 700, sensor_width, sensor_height*2);
	sensor1->listener = this;

	// S2 (Checkpoint): Esquina superior derecha, línea horizontal
	sensor2 = App->physics->CreateRectangleSensor(430, 200, sensor_height * 1.5, sensor_width);
	sensor2->listener = this;

	// S3 (Checkpoint): Esquina inferior derecha, línea vertical
	sensor3 = App->physics->CreateRectangleSensor(1400, 100, sensor_width, sensor_height*2);
	sensor3->listener = this;

	// S4 (Checkpoint): Esquina inferior izquierda, línea horizontal
	sensor4 = App->physics->CreateRectangleSensor(1580, 580, sensor_height * 2.7, sensor_width);
	sensor4->listener = this;

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
	if (game_over) {
		// Si el juego ha terminado, no procesamos la entrada ni actualizamos entidades.
		return UPDATE_CONTINUE;
	}

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

update_status ModuleGame::PostUpdate()
{
	// Llama a la Update() base por si tiene lógica.
	update_status ret = Module::PostUpdate();

	int current_lap = laps + 1;

	if (laps == 0 && (lap_progress_state & 1) != 1) { // Si laps es 0 y S1 (bit 1) no ha sido tocado aún
		current_lap = 1; // Se considera que est?en la vuelta 1, aunque laps sea 0.
	}
	else {
		current_lap = laps + 1;
	}

	// --- CONVERSIÓN DE ENTERO A TEXTO ---
	// Buffer para construir la cadena de texto: "Laps: X"
	static char laps_text[64];
	sprintf_s(laps_text, 64, "Vueltas: %d", laps);

	// --- DIBUJAR TEXTO (Usando DrawText simple de Raylib) ---
	// Dibuja el texto en la esquina superior izquierda (ejemplo: 20px, 20px)
	// El tamaño de la fuente es 20, y el color es blanco.

	// La cámara del renderer debe estar ajustada para esto, pero DrawText debería
	// dibujar en coordenadas de pantalla (no de mundo) por defecto.

	DrawText(laps_text, 20, 20, 20, WHITE);

	// Opcionalmente, mostrar el estado de progreso
	static char progress_text[64];
	sprintf_s(progress_text, 64, "Progreso: %d", lap_progress_state);
	DrawText(progress_text, 20, 50, 20, YELLOW);

	if (laps >= 2) // Comprobamos si las vueltas son 8 o más
	{
		// Dibuja el mensaje de "WIN" en el centro de la pantalla
		DrawText("WIN", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 50, GOLD);

		return UPDATE_CONTINUE;
	}

	return ret;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	PhysBody* sensor = nullptr;
	PhysBody* other = nullptr;

	// 1. Identificar cuál cuerpo es el sensor y cuál es el coche a seguir.
	if ((bodyA == sensor1 || bodyA == sensor2 || bodyA == sensor3 || bodyA == sensor4) && bodyB == car_to_track) {
		sensor = bodyA;
		other = bodyB;
	}
	else if ((bodyB == sensor1 || bodyB == sensor2 || bodyB == sensor3 || bodyB == sensor4) && bodyA == car_to_track) {
		sensor = bodyB;
		other = bodyA;
	}

	if (sensor && other == car_to_track) {
		// 2. Definir los bits para el estado de progreso
		int sensor_bit = 0;
		const int S1 = 1, S2 = 2, S3 = 4, S4 = 8;
		const int ALL_SENSORS_HIT = S1 | S2 | S3 | S4; // 1 + 2 + 4 + 8 = 15

		if (sensor == sensor1) sensor_bit = S1;
		else if (sensor == sensor2) sensor_bit = S2;
		else if (sensor == sensor3) sensor_bit = S3;
		else if (sensor == sensor4) sensor_bit = S4;

		if (sensor_bit != 0) {

			if (sensor == sensor1) {
				// S1 es la línea de INICIO/FIN
				if (lap_progress_state == ALL_SENSORS_HIT) {
					// FIN DE VUELTA: Todos los checkpoints fueron tocados y S1 es tocado de nuevo.
					laps++;
					if (laps == 2) {
						game_over = true;
						//DrawText("WIN", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 50, GOLD);
					}
					lap_progress_state = S1; // Reiniciar progreso a S1 tocado (inicio de la próxima vuelta)
					LOG("--- VUELTA COMPLETADA! Vueltas Totales: %d ---", laps);
				}
				else {
					// INICIO/REINICIO: Se toca S1 antes de completar la vuelta. Se reinicia el progreso
					// a solo S1 tocado, forzando la secuencia a empezar de nuevo.
					lap_progress_state = S1;
					LOG("Coche inici?reinici?vuelta en Sensor 1. Estado de Progreso: %d", lap_progress_state);
				}
			}
			else if ((lap_progress_state & S1) == S1) {
				// 3. Progresar: Solo se permite progresar si S1 (Inicio) ya ha sido tocado.

				// S2 debe ser tocado después de S1. Si S1 est?en 1 y S2 no.
				if (sensor == sensor2 && lap_progress_state == S1) {
					lap_progress_state |= S2;
					LOG("Coche toc?Sensor 2. Estado de Progreso: %d", lap_progress_state);
				}
				// S3 debe ser tocado después de S2 (es decir, si S2 est?en el estado).
				else if (sensor == sensor3 && (lap_progress_state & S2) && !(lap_progress_state & S3)) {
					lap_progress_state |= S3;
					LOG("Coche toc?Sensor 3. Estado de Progreso: %d", lap_progress_state);
				}
				// S4 debe ser tocado después de S3 (es decir, si S3 est?en el estado).
				else if (sensor == sensor4 && (lap_progress_state & S3) && !(lap_progress_state & S4)) {
					lap_progress_state |= S4;
					LOG("Coche toc?Sensor 4. Estado de Progreso: %d", lap_progress_state);
				}
			}
		}
	}
}
