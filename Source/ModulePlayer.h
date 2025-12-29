#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB) override;
public:
	PhysBody* pbody;
	Texture2D texture;

	int x;
	int y;
	int width;
	int height;
	float speed;
	float turn_speed;
	// 增加倒计时
	int boostTimer = 0;
	float currentMaxSpeed = 0.0f;

	float currentBoostCharge = 0.0f; // 当前充能进度 (累计行驶距离)
	float maxBoostCharge = 0.0f;     // 需要行驶的总距离 (即 100% 进度)

	int warningTimer = 0;

	bool showHelpMenu = false;
}; 
