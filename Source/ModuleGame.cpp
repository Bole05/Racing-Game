#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleMap.h"
#include "ModulePlayer.h"
#include"ModuleAi.h"
#include <algorithm>
struct RankingInfo {
	const char* name;
	int score;
	bool isPlayer;
};
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

	//App->renderer->camera.x = App->renderer->camera.y = 0;
	App->renderer->camera.target = { 0.0f, 0.0f };
	App->renderer->camera.offset = { 0.0f, 0.0f };
	App->renderer->camera.zoom = 1.0f;
	App->map->Load("Assets-racing/Maps/MapTemplate.tmx");
	// App->player->pbody->body->SetTransform(App->map->playerSpawnPoint, 0);

	App->audio->PlayMusic("Assets-racing/Audio/Music/action-racing-speed-music-380058.wav");

	finishFx = App->audio->LoadFx("Assets-racing/Audio/Music/vueltaCompletada.wav");

	// 2. 设置玩家位置 (修复重点：取消注释并转换坐标单位)
	if (App->player->pbody != nullptr) {
		// 必须将 Tiled 的像素坐标转换为物理引擎的米坐标
		float metersX = PIXEL_TO_METERS(App->map->playerSpawnPoint.x);
		float metersY = PIXEL_TO_METERS(App->map->playerSpawnPoint.y);

		App->player->pbody->body->SetTransform(b2Vec2(metersX, metersY), -90.0f * DEGTORAD);
		LOG("Player moved to spawn point (meters): %f, %f", metersX, metersY);
	}

	
	// 2. 生成敌人
	for (const auto& spawnPos : App->map->enemySpawnPoints)
	{
		// 这里的 0 是路径索引，你可能需要逻辑来计算该生成点离哪个路径点最近
		App->Ai->CreateEnemyAtPosition(spawnPos, 0);
	}

	//App->Ai->CreateEnemy(0);
	
	car_to_track = App->player->pbody;
	

	//// Crear un par de enemigos en posiciones diferentes de la ruta
	//App->Ai->CreateEnemy(0);  // Empieza en el punto 0
	//App->Ai->CreateEnemy(5);  // Empieza en el punto 5 (m醩 adelante)
	

	int sensor_width = 10;
	int sensor_height = 100;

	// S1 (Inicio/Fin): cerca de la posici髇 inicial del coche, l韓ea vertical
	sensor1 = App->physics->CreateRectangleSensor(600, 700, sensor_width, sensor_height*2);
	sensor1->listener = this;

	// S2 (Checkpoint): Esquina superior derecha, l韓ea horizontal
	sensor2 = App->physics->CreateRectangleSensor(430, 200, sensor_height * 1.5, sensor_width);
	sensor2->listener = this;

	// S3 (Checkpoint): Esquina inferior derecha, l韓ea vertical
	sensor3 = App->physics->CreateRectangleSensor(1400, 100, sensor_width, sensor_height*2);
	sensor3->listener = this;

	// S4 (Checkpoint): Esquina inferior izquierda, l韓ea horizontal
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

	if (game_over && IsKeyPressed(KEY_R)) {
		game_over = false;
		laps = 0;
		lap_progress_state = 0;

		// Llamamos a la funci髇 que acabas de crear
		//App->Ai->ResetEnemies();

		// Resetear al Jugador (usando sus coordenadas de spawn)
		if (App->player->pbody != nullptr) {
			float metersX = PIXEL_TO_METERS(App->map->playerSpawnPoint.x);
			float metersY = PIXEL_TO_METERS(App->map->playerSpawnPoint.y);
			App->player->pbody->body->SetTransform({ metersX, metersY }, -90.0f * DEGTORAD);
			App->player->pbody->body->SetLinearVelocity({ 0, 0 });
		}
	}

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
	// Llama a la Update() base por si tiene l骻ica.
	update_status ret = Module::PostUpdate();

	int current_lap = laps + 1;

	if (laps == 0 && (lap_progress_state & 1) != 1) { // Si laps es 0 y S1 (bit 1) no ha sido tocado a鷑
		current_lap = 1; // Se considera que est?en la vuelta 1, aunque laps sea 0.
	}
	else {
		current_lap = laps + 1;
	}

	EndMode2D();
	// --- CONVERSI覰 DE ENTERO A TEXTO ---
	// Buffer para construir la cadena de texto: "Laps: X"
	static char laps_text[64];
	sprintf_s(laps_text, 64, "Vueltas: %d", laps);

	// --- DIBUJAR TEXTO (Usando DrawText simple de Raylib) ---
	// Dibuja el texto en la esquina superior izquierda (ejemplo: 20px, 20px)
	// El tama駉 de la fuente es 20, y el color es blanco.

	// La c醡ara del renderer debe estar ajustada para esto, pero DrawText deber韆
	// dibujar en coordenadas de pantalla (no de mundo) por defecto.

	DrawText(laps_text, 20, 20, 20, WHITE);

	// Opcionalmente, mostrar el estado de progreso
	static char progress_text[64];
	sprintf_s(progress_text, 64, "Progreso: %d", lap_progress_state);
	DrawText(progress_text, 20, 50, 20, YELLOW);





	// 1. 收集数据
	std::vector<RankingInfo> leaderboard;

	// 添加玩家
	int playerSensors = 0;
	if (lap_progress_state & 8) playerSensors = 4;
	else if (lap_progress_state & 4) playerSensors = 3;
	else if (lap_progress_state & 2) playerSensors = 2;
	else if (lap_progress_state & 1) playerSensors = 1;
	leaderboard.push_back({ "Player", laps * 10 + playerSensors, true });

	// 添加 AI
	for (int i = 0; i < App->Ai->enemies.size(); ++i) {
		auto& e = App->Ai->enemies[i];
		int aiSensors = 0;
		if (e.lap_progress_state & 8) aiSensors = 4;
		else if (e.lap_progress_state & 4) aiSensors = 3;
		else if (e.lap_progress_state & 2) aiSensors = 2;
		else if (e.lap_progress_state & 1) aiSensors = 1;

		static char aiName[16];
		sprintf_s(aiName, "Enemy %d", i + 1);
		leaderboard.push_back({ aiName, e.laps * 10 + aiSensors, false });
	}

	// 2. 排序（分数高的在前）
	std::sort(leaderboard.begin(), leaderboard.end(), [](const RankingInfo& a, const RankingInfo& b) {
		return a.score > b.score;
		});

	// 3. 绘制排行榜界面
	int py = 100;
	int playerRank = 0;
	DrawText("--- RANKING ---", 20, py, 20, GOLD);
	for (int i = 0; i < leaderboard.size(); ++i) {
		py += 25;
		Color color = leaderboard[i].isPlayer ? GREEN : WHITE;
		if (leaderboard[i].isPlayer) playerRank = i + 1;

		static char rankText[64];
		sprintf_s(rankText, "%d. %s (Laps: %d)", i + 1, leaderboard[i].name, leaderboard[i].score / 10);
		DrawText(rankText, 20, py, 20, color);
	}

	// 在屏幕显眼位置显示玩家当前排名
	static char currentRankStr[32];
	sprintf_s(currentRankStr, "POS: %d / %d", playerRank, (int)leaderboard.size());
	DrawText(currentRankStr, SCREEN_WIDTH - 150, 20, 30, ORANGE);

	if (laps >= 2) // Comprobamos si las vueltas son 8 o m醩
	{
		// Dibuja el mensaje de "WIN" en el centro de la pantalla
		DrawText("WIN", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 50, GOLD);

		return UPDATE_CONTINUE;
	}

	if (game_over)
	{
		// Dibujamos un fondo oscuro para que resalte el texto final
		DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));

		// Comprobamos qui閚 est?en la posici髇 1 del leaderboard que ya calculaste arriba
		if (!leaderboard.empty()) {
			if (leaderboard[0].isPlayer) {
				// Si el primero de la lista es el jugador
				DrawText("YOU WIN!", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 100, 60, GOLD);
			}
			else {
				// Si el primero es un enemigo
				DrawText("LOSE", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 100, 60, RED);
			}

			// Mostramos tu ranking final en grande
			char finalPos[32];
			sprintf_s(finalPos, "FINAL RANKING: %d", playerRank);
			DrawText(finalPos, SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2, 30, WHITE);

			DrawText("PRESS R TO RESTART", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 20, GRAY);
		}

		return UPDATE_CONTINUE;
	}

	return ret;
}


void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	PhysBody* sensor = nullptr;
	PhysBody* vehicleBody = nullptr;

	if (game_over) return;

	// 1. 识别哪个是传感器，哪个是车辆
	if (bodyA == sensor1 || bodyA == sensor2 || bodyA == sensor3 || bodyA == sensor4) {
		sensor = bodyA;
		vehicleBody = bodyB;
	}
	else if (bodyB == sensor1 || bodyB == sensor2 || bodyB == sensor3 || bodyB == sensor4) {
		sensor = bodyB;
		vehicleBody = bodyA;
	}

	if (sensor && vehicleBody) {
		// 2. 获取进位掩码
		int sensor_bit = 0;
		const int S1 = 1, S2 = 2, S3 = 4, S4 = 8;
		const int ALL_SENSORS_HIT = 15;

		if (sensor == sensor1) sensor_bit = S1;
		else if (sensor == sensor2) sensor_bit = S2;
		else if (sensor == sensor3) sensor_bit = S3;
		else if (sensor == sensor4) sensor_bit = S4;

		// 3. 处理逻辑（提取成 lambda 或函数以复用）
		auto UpdateLapLogic = [&](int& vehicleLaps, int& vehicleProgress) {
			if (sensor == sensor1) {
				if (vehicleProgress == ALL_SENSORS_HIT) {
					vehicleLaps++;
					vehicleProgress = S1;

					if (vehicleLaps >= 2) { // Si alguien llega a las vueltas necesarias (ej: 2)
						this->game_over = true;
						if (vehicleBody == car_to_track) {
							// El jugador gan?(esto lo manejaremos en el dibujo)
						}
						else {
							LOG("Un enemigo ha ganado. GAME OVER.");
						}
					}

					if (vehicleBody == car_to_track) {
						App->audio->PlayFx(finishFx);
					}
					LOG("Vehicle completed a lap! Total: %d", vehicleLaps);
				}
				else {
					vehicleProgress = S1;
				}
			}
			else if ((vehicleProgress & S1) == S1) {
				if (sensor == sensor2 && vehicleProgress == S1) vehicleProgress |= S2;
				else if (sensor == sensor3 && (vehicleProgress & S2) && !(vehicleProgress & S3)) vehicleProgress |= S3;
				else if (sensor == sensor4 && (vehicleProgress & S3) && !(vehicleProgress & S4)) vehicleProgress |= S4;
			}
			};

		// 判断是玩家还是 AI
		if (vehicleBody == car_to_track) {
			UpdateLapLogic(laps, lap_progress_state);
		}
		else {
			for (auto& enemy : App->Ai->enemies) {
				if (vehicleBody == enemy.pbody) {
					UpdateLapLogic(enemy.laps, enemy.lap_progress_state);
					break;
				}
			}
		}
	}
}