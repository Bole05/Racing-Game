#include "ModuleAi.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleMap.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModulePlayer.h"
#include "CarProperties.h"

#include <cmath>

// Definiciones de seguridad
#ifndef DEGTORAD
#define DEGTORAD 0.0174532925199432957f
#endif
#ifndef RAD2DEG
#define RAD2DEG 57.295779513082320876f
#endif

class AICallback : public b2RayCastCallback {
public:
    b2Fixture* fixture = nullptr;
    b2Vec2 point;
    b2Vec2 normal;
    float fraction = 1.0f;
    bool hitPlayer = false;

    Module* playerModule = nullptr;
    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
        if (fixture->IsSensor()) return -1.0f;
        //para caso de que el raycast un de susline encuentra el player y implementar estos codis para que el raycasr conoce que ese objeto es player
        b2Body* body = fixture->GetBody();
        if (body) {
            PhysBody* pbody = (PhysBody*)body->GetUserData().pointer;
            if (pbody && playerModule != nullptr && pbody->listener == playerModule) {
                hitPlayer = true;
            }
            else {
                hitPlayer = false;
            }
        }
        



        this->fixture = fixture;
        this->point = point;
        this->normal = normal;
        this->fraction = fraction;
        return fraction;
    }
};

void EnemyCar::Init(PhysBody* body, int startPathIndex,int pathIndex) {
    pbody = body;
    currentPathIndex = startPathIndex;
    selectedPathIndex = pathIndex;
    active = true;
    if (pbody && pbody->body) {
        // Aumentamos damping angular para reducir el "trote" (vibraci髇)
        pbody->body->SetLinearDamping(0.3f);
        pbody->body->SetAngularDamping(4.0f); // Alto para evitar giros locos
    }
}

ModuleAi::ModuleAi(Application* app, bool start_enabled) : Module(app, start_enabled) {}
ModuleAi::~ModuleAi() {}

bool ModuleAi::Start()
{
    LOG("Loading AI");
    texture = LoadTexture("Assets-racing/Textures/Car1.png");
    return true;
}

void ModuleAi::CreateEnemy(int startPathIndex)
{
    int randomPathID = rand() % App->map->trackPaths.size();
    if (App->map->trackPaths[randomPathID].empty()) return;

    b2Vec2 startPos = App->map->trackPaths[randomPathID][startPathIndex];

    // 创建矩形碰撞体
    PhysBody* pb = App->physics->CreateRectangle(
        METERS_TO_PIXELS(startPos.x),
        METERS_TO_PIXELS(startPos.y),
        26, 43,
        1, 0xFFFF // 这里的 0xFFFF 允许 AI 碰撞所有物体，包括传感器
    );

    if (pb != nullptr) {
        pb->body->SetTransform(pb->body->GetPosition(), -90.0f * DEGTORAD);

        // --- 为了公平性，添加以下物理属性设置 ---

        // 1. 设置和玩家一样的阻尼 (Damping)
        pb->body->SetLinearDamping(CarStats::LINEAR_DAMPING);
        pb->body->SetAngularDamping(CarStats::ANGULAR_DAMPING);

        // 2. 设置物理材质属性
        b2Fixture* fixture = pb->body->GetFixtureList();
        if (fixture) {
            fixture->SetDensity(CarStats::DENSITY);     // 确保重量一致
            fixture->SetFriction(CarStats::FRICTION);   // 确保摩擦一致
            fixture->SetRestitution(CarStats::RESTITUTION); // 确保碰撞弹性一致

            // 重要：改变密度后必须重置质量，否则 AI 会感觉很轻或很重
            pb->body->ResetMassData();
        }
    }

    EnemyCar enemy;
    enemy.Init(pb, startPathIndex, randomPathID);

    enemy.maxSpeed = CarStats::MAX_SPEED;
    enemy.turnSpeed = CarStats::STEERING_SPEED; 
    enemy.width = 26;
    enemy.height = 43;

    // 初始化圈数计数变量（如果你已经在 EnemyCar 结构体里添加了它们）
    enemy.laps = 0;
    enemy.lap_progress_state = 0;

    enemies.push_back(enemy);
}

float ModuleAi::CastRay(b2Body* body, float rayLength, float angleOffset, int colorType, bool& outHitPlayer)
{
    outHitPlayer = false;
    b2World* world = body->GetWorld();
    if (!world) return 1.0f;

    b2Vec2 start = body->GetPosition();
    float angle = body->GetAngle() + angleOffset;

    b2Vec2 direction;
    direction.x = sin(angle);
    direction.y = -cos(angle);

    b2Vec2 end;
    end.x = start.x + (direction.x * rayLength);
    end.y = start.y + (direction.y * rayLength);

    AICallback callback;
    callback.playerModule = App->player;
    world->RayCast(&callback, start, end);
    if (callback.fixture) {
        outHitPlayer = callback.hitPlayer;
    }

    if (App->physics->debug) {
        Color c = (colorType == 0) ? YELLOW : ORANGE;
        if (callback.fixture) c = RED;
        if (callback.hitPlayer)c = PURPLE;
        b2Vec2 drawEnd = (callback.fixture) ? callback.point : end;

        DebugLine line;
        line.x1 = METERS_TO_PIXELS(start.x);
        line.y1 = METERS_TO_PIXELS(start.y);
        line.x2 = METERS_TO_PIXELS(drawEnd.x);
        line.y2 = METERS_TO_PIXELS(drawEnd.y);
        line.color = c;

        debugLines.push_back(line);
        //DrawLine(METERS_TO_PIXELS(start.x), METERS_TO_PIXELS(start.y),
        //    METERS_TO_PIXELS(drawEnd.x), METERS_TO_PIXELS(drawEnd.y), c);
    }
    return callback.fraction;
}

update_status ModuleAi::Update()
{
    if (App->game != nullptr && App->game->game_over) return UPDATE_CONTINUE;
    if (App->map->trackPaths.empty()) return UPDATE_CONTINUE;
    for (auto& car : enemies)
    {
        if (!car.active || !car.pbody) continue;
        const std::vector<b2Vec2>& path = App->map->trackPaths[car.selectedPathIndex];
        if (path.empty()) continue;
        b2Body* b = car.pbody->body;
        b2Vec2 position = b->GetPosition();

        // --- 1. SEGUIMIENTO DE RUTA ROBUSTO ---

        // Buscar el punto m醩 cercano en toda la ruta (para no perderse nunca)
        // Optimizamos buscando solo en los siguientes 5 puntos para no iterar todo siempre
        //---------------------------Seguimiento Ruta------------------------------------------------------------------------------------//
        int bestIndex = car.currentPathIndex;
        float minDistance = 100000.0f;

        for (int i = 0; i < 5; i++) {
            int idx = (car.currentPathIndex + i) % path.size();
            b2Vec2 diff = path[idx] - position;
            float dist = diff.Length();
            if (dist < minDistance) {
                minDistance = dist;
                bestIndex = idx;
            }
        }
        car.currentPathIndex = bestIndex;

        // TARGET: Miramos 2 puntos por delante para cortar curvas suavemente
        int targetIndex = (car.currentPathIndex + 2) % path.size();
        b2Vec2 target = path[targetIndex];

        b2Vec2 diff = target - position;
        float desiredAngle = atan2f(diff.y, diff.x) + (b2_pi / 2.0f);

        // --- 2. RayCast ---
      /*  float speed = b->GetLinearVelocity().Length();*/
        float lookAhead = 4.0f; // Rayos fijos para estabilidad
        float sideLookAhead = 2.5f;

        bool hitPlayerLeft = false;
        bool hitPlayerRight = false;
        bool hitPlayerFront = false;


        float leftSpace = CastRay(b, sideLookAhead, -30.0f * DEGTORAD, 0,hitPlayerLeft);
        float rightSpace = CastRay(b, sideLookAhead, 30.0f * DEGTORAD, 0,hitPlayerRight);
        float frontSpace = CastRay(b, lookAhead * 1.2f, 0.0f, 1,hitPlayerFront);

        float finalAngle = desiredAngle;
        float avoidanceFactor = 0.0f;

        // Si detectamos pared, mezclamos el 醤gulo deseado con la correcci髇
        // No reemplazamos totalmente el 醤gulo, solo lo empujamos
        if (leftSpace < 0.9f) {
            // 如果左边是玩家，AI可能会更激进（挤压玩家），如果是墙则避让
            float force = (hitPlayerLeft) ? 0.5f : 2.0f;
            avoidanceFactor += (0.8f - leftSpace) * force;
        }
        if (rightSpace < 0.9f) {
            float force = (hitPlayerRight) ? 0.5f : 2.0f;
            avoidanceFactor -= (0.8f - rightSpace) * force;
        }

        finalAngle += avoidanceFactor;

        // --- 3. GIRO ---
        float currentAngle = b->GetAngle();
        float nextAngle = finalAngle - currentAngle;
        while (nextAngle <= -b2_pi) nextAngle += 2 * b2_pi;
        while (nextAngle > b2_pi) nextAngle -= 2 * b2_pi;

        // Giramos con Clamp para evitar que el volante se vuelva loco
        float turnSpeed = 4.0f;
        b->SetAngularVelocity(nextAngle * turnSpeed);



        //--------------Calcular la curvatura de la carrera

        float pathAngleDiff = desiredAngle - currentAngle;
        while (pathAngleDiff <= -b2_pi) pathAngleDiff += 2 * b2_pi;
        while (pathAngleDiff > b2_pi) pathAngleDiff -= 2 * b2_pi;

        float turnDrag = 1.0f;
        if (abs(pathAngleDiff) > 0.6f) {
            turnDrag = 0.7f;
        }

        // 4.3 只有当障碍物出现在“正前方”时，才剧烈刹车
        // 侧面有墙(left/rightSpace)不会触发这里
        if (frontSpace < 0.6f) {
            if (hitPlayerFront) {
                turnDrag *= 0.7f; // 撞玩家轻微减速
            }
            else {
                turnDrag *= 0.4f; // 撞墙必须重刹
            }
        }




        // --- 4. MOVIMIENTO (CORREGIDO PARA CURVAS) ---
        b2Vec2 forwardVec;
        forwardVec.x = sin(b->GetAngle());
        forwardVec.y = -cos(b->GetAngle());

        float speed = b->GetLinearVelocity().Length();
        float currentMaxSpeed = car.maxSpeed * turnDrag;
        // --- L骻ica Anti-Atasco (Stuck) MENOS SENSIBLE ---
        // Solo activamos si la velocidad es casi CERO (0.2)

        if (speed < 0.2f) car.stuckTimer++;
        else car.stuckTimer = 0;

        if (car.stuckTimer > 60) {
            // Maniobra de rescate
            b2Vec2 reverseForce;
            reverseForce.x = forwardVec.x * -6.0f;
            reverseForce.y = forwardVec.y * -6.0f;
            b->ApplyForceToCenter(reverseForce, true);

            // Girar mientras retrocede para desencajarse
            b->SetAngularVelocity(2.0f);

            if (car.stuckTimer > 120) car.stuckTimer = 0;
        }
        else {

            // Aceleramos
            if (speed < currentMaxSpeed) {
                b2Vec2 driveForce;
                driveForce.x = forwardVec.x * 15.0f;
                driveForce.y = forwardVec.y * 15.0f;
                b->ApplyForceToCenter(driveForce, true);
            }
        }

        KillOrthogonalVelocity(b);
    }

    return UPDATE_CONTINUE;
}

void ModuleAi::KillOrthogonalVelocity(b2Body* body)
{
    float currentAngle = body->GetAngle();
    // 注意：这里的 forwardDir 必须和玩家定义的计算方式完全一致
    b2Vec2 forwardDir = { (float)sin(currentAngle), (float)-cos(currentAngle) };

    b2Vec2 currentVel = body->GetLinearVelocity();
    float forwardSpeed = b2Dot(currentVel, forwardDir);

    // AI 也只保留向前速度，消除侧向速度
    body->SetLinearVelocity(forwardSpeed * forwardDir);
}

update_status ModuleAi::PostUpdate()
{
    for (auto& car : enemies) {
        if (car.active && car.pbody != nullptr) {
            int posX, posY;
            car.pbody->GetPhysicPosition(posX, posY);
            float rotationDegrees = car.pbody->GetRotation() * RAD2DEG;
            Rectangle sourceRec = { 0.0f, 0.0f, (float)car.width, (float)car.height };
            Rectangle destRec = { (float)posX, (float)posY, (float)car.width, (float)car.height };
            Vector2 origin = { (float)car.width / 2, (float)car.height / 2 };
            DrawTexturePro(texture, sourceRec, destRec, origin, rotationDegrees, RED);

        }
    }

    if (App->physics->debug) {
        // 1. 绘制缓存的射线
        for (const auto& line : debugLines) {
            DrawLine(line.x1, line.y1, line.x2, line.y2, line.color);
        }
        // 2. 清空列表，为下一帧做准备
        debugLines.clear();

        // 绘制路径点（保留你原来的代码）
        if (!App->map->trackPaths.empty()) {
            for (const auto& path : App->map->trackPaths) {
                for (const auto& p : path) {
                    DrawCircle(METERS_TO_PIXELS(p.x), METERS_TO_PIXELS(p.y), 3, GREEN);
                }
            }
        }
    }
    return UPDATE_CONTINUE;
}

void ModuleAi::CreateEnemyAtPosition(b2Vec2 position, int startPathIndex)
{
    // 创建物理身体
    // 注意：确保 METERS_TO_PIXELS 转换逻辑与你的地图坐标系统一致
    // 如果 Tiled 里的坐标已经是像素，这里可能不需要转换，或者取决于你如何读取的 x,y
    PhysBody* pb = App->physics->CreateRectangle(
        position.x,
        position.y,
        26, 43,
        1, 0xFFFF
    );

    if (pb != nullptr) {
        pb->body->SetTransform(pb->body->GetPosition(), -90.0f * DEGTORAD);
    }
    EnemyCar enemy;
    // 这里传入 0 或者最近的路径点索引
    int defaultPathIndex = 0;
    enemy.Init(pb, startPathIndex, defaultPathIndex);
    enemy.width = 26;
    enemy.height = 43;
    enemies.push_back(enemy);
}



bool ModuleAi::CleanUp()
{
    UnloadTexture(texture);
    enemies.clear();
    return true;
}