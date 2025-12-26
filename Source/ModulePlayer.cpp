#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleGame.h"
#include "ModuleMap.h"
#include "CarProperties.h"
ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{
}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	texture = LoadTexture("Assets-racing/Textures/Car1.png");

   
    int frameWidth = 26;
    int frameHeight = 43;
    x = App->map->playerSpawnPoint.x;
    y = App->map->playerSpawnPoint.y;

    if (x == 0 && y == 0) {
        x = 100; // 默认值
        y = 300;
        LOG("Warning: No PlayerStart found in map, using default.");
    }

    pbody = App->physics->CreateRectangle(x, y, frameWidth, frameHeight, 1, 0xFFFF);

    this->width = frameWidth;
    this->height = frameHeight;

    if (pbody != nullptr)
    {
        pbody->listener = this;
        // 使用 CarStats 中定义的统一数值
        pbody->body->SetLinearDamping(CarStats::LINEAR_DAMPING);
        pbody->body->SetAngularDamping(CarStats::ANGULAR_DAMPING);

        // 获取并修改 Fixture 属性 (影响质量和碰撞反应)
        b2Fixture* fixture = pbody->body->GetFixtureList();
        if (fixture) {
            fixture->SetDensity(CarStats::DENSITY);
            fixture->SetFriction(CarStats::FRICTION);
            fixture->SetRestitution(CarStats::RESTITUTION);
            pbody->body->ResetMassData(); // 修改密度后必须重置
        }
    }

    // 使用 CarStats 中定义的动力数值
    speed = CarStats::ACCELERATION;
    turn_speed = CarStats::STEERING_SPEED;


    currentMaxSpeed = CarStats::MAX_SPEED; // 初始速度
    boostTimer = 0;
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
    UnloadTexture(texture);
	return true;
}
update_status ModulePlayer::Update()
{
    if (App->game != nullptr && App->game->game_over == true) return UPDATE_CONTINUE;

    if (pbody != nullptr)
    {
        // 1. 处理加速计时逻辑
        if (boostTimer > 0) {
            boostTimer--;
            if (boostTimer == 0) {
                currentMaxSpeed = CarStats::MAX_SPEED; // 时间到，恢复原始速度
                LOG("Speed normal");
            }
        }

        // 2. 使用 currentMaxSpeed 而不是固定值
        // 如果 currentMaxSpeed 还没初始化，就在 Start 里设置 currentMaxSpeed = CarStats::MAX_SPEED
        float maxSpeed = currentMaxSpeed;






        b2Body* b = pbody->body;

        // 1. OBTENER DIRECCI?N Y VELOCIDAD
        float currentAngle = b->GetAngle();
        // IMPORTANTE: Esta fórmula debe ser igual en la IA
        b2Vec2 forwardDir = { (float)sin(currentAngle), (float)-cos(currentAngle) };
        b2Vec2 currentVel = b->GetLinearVelocity();
        float forwardSpeed = currentVel.x * forwardDir.x + currentVel.y * forwardDir.y;

        // 2. FRICCI?N LATERAL (SIN DERRAPE)
        // Sincronizado con KillOrthogonalVelocity de la IA
        b2Vec2 lateralCorrection = { forwardDir.x * forwardSpeed, forwardDir.y * forwardSpeed };
        b->SetLinearVelocity(lateralCorrection);

        // 3. GIRO (STEERING)
        float targetRotVelocity = 0.0f;
        float maxTurnSpeed = CarStats::STEERING_SPEED;

        if (abs(forwardSpeed) > 0.5f) // Mínimo de velocidad para girar
        {
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) targetRotVelocity = -maxTurnSpeed;
            else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) targetRotVelocity = maxTurnSpeed;

            if (forwardSpeed < -0.1f) targetRotVelocity *= -1; // Invertir en reversa
        }

        // Sincroniza este valor (0.2f) con la IA si quieres que giren con la misma agilidad
        float turnSmoothing = 0.2f;
        float currentRot = b->GetAngularVelocity();
        float newRot = currentRot + (targetRotVelocity - currentRot) * turnSmoothing;
        b->SetAngularVelocity(newRot);

        // 4. ACELERACI?N
    /*    float maxSpeed = CarStats::MAX_SPEED;*/

        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        {
            if (forwardSpeed < maxSpeed) {
                // 'speed' ya es CarStats::ACCELERATION (asignado en Start)
                b2Vec2 force = { forwardDir.x * speed, forwardDir.y * speed };
                b->ApplyForceToCenter(force, true);
            }
        }

        // 5. FRENADO Y REVERSA
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        {
            if (forwardSpeed > -maxSpeed * 0.5f) {
                b2Vec2 force = { forwardDir.x * -speed * 0.5f, forwardDir.y * -speed * 0.5f };
                b->ApplyForceToCenter(force, true);
            }
        }

        // 6. FRICCI?N NATURAL (Cuando no aceleras)
        if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_SPACE)) {
            b2Vec2 v = b->GetLinearVelocity();
            v.x *= 0.98f; // Sincroniza este 0.98f con la IA
            v.y *= 0.98f;
            b->SetLinearVelocity(v);
        }
    }

    return UPDATE_CONTINUE;
}



update_status ModulePlayer::PostUpdate()
{
    if (pbody != nullptr)
    {
        // --- RENDERIZADO (RAYLIB) ---

        // 1. Obtener posici髇 actualizada de las f韘icas
        int posX, posY;
        pbody->GetPhysicPosition(posX, posY);

        // 2. Obtener rotaci髇
        float rotationDegrees = pbody->GetRotation() * RAD2DEG;

        // 3. Dibujar
        Rectangle sourceRec = { 0.0f, 0.0f, (float)this->width,(float)this->height };
        Rectangle destRec = { (float)posX, (float)posY, (float)this->width, (float)this->height };
        Vector2 origin = { (float)this->width / 2, (float)this->height / 2 };

        DrawTexturePro(texture, sourceRec, destRec, origin, rotationDegrees, WHITE);
    }

    return UPDATE_CONTINUE;
}

// ModulePlayer.cpp
void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
    // bodyB 是玩家撞到的东西
    if (bodyB != nullptr && bodyB->ptype == BodyType::BOOST)
    {
        boostTimer = 60; // 3秒加速 (假设60fps)
        currentMaxSpeed = CarStats::MAX_SPEED +3.0f; // 速度翻倍
        LOG("BOOST ACTIVE!");
    }
}