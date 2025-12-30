#pragma once

#include "Globals.h"
#include "Module.h"

#include "p2Point.h"
#include "pugixml.hpp"
#include "raylib.h"
#include <vector>

class PhysBody;
class PhysicEntity;

enum GameState {
	START_MENU,
	COUNTDOWN,
	INGAME,
	GAME_OVER
};

class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	bool game_over = false;
	bool is_paused = false;

	GameState current_state = START_MENU; // Empezamos en el men?
	Texture2D menu_img;

	uint finishFx = 0;
	

public:

	std::vector<PhysicEntity*> entities;
	
	Texture2D plane;
	Texture2D car;
	Texture2D ship;
	Texture2D bike;

	vec2<int> ray;
	bool ray_on;

	// Variables para el sistema de vueltas
	PhysBody* car_to_track = nullptr; // El coche al que se le har?el seguimiento
	PhysBody* sensor1 = nullptr; // Sensor de Inicio/Fin
	PhysBody* sensor2 = nullptr; // Checkpoint 2
	PhysBody* sensor3 = nullptr; // Checkpoint 3
	PhysBody* sensor4 = nullptr; // Checkpoint 4

	// Estado de progreso: Bitmask para S1=1, S2=2, S3=4, S4=8
	int lap_progress_state = 0;
	int laps = 0;

	float countdown_timer = 3.0f;
};
