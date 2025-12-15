//#include "ModuleAi.h"
//#include "Application.h"
//#include "ModulePhysics.h"
//#include "ModuleMap.h"
//#include "ModuleRender.h"
//#include "ModuleGame.h"
//
//class AICallback : public b2RayCastCallback {
//public:
//    b2Fixture* fixture = nullptr;
//    b2Vec2 point;
//    b2Vec2 normal;
//    float fraction = 1.0f;
//
//    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
//        if (fixture->IsSensor()) return -1.0f; // Ignorar sensores
//        this->fixture = fixture;
//        this->point = point;
//        this->normal = normal;
//        this->fraction = fraction;
//        return fraction;
//    }
//};
//
//void EnemyCar::Init(PhysBody* body, int startPathIndex) {
//    pbody = body;
//    currentPathIndex = startPathIndex;
//    active = true;
//
//    // Como hemos incluido ModulePhysics.h, ahora s?podemos acceder a 'body'
//    if (pbody != nullptr && pbody->body != nullptr) {
//        pbody->body->SetLinearDamping(0.4f);
//        pbody->body->SetAngularDamping(2.0f);
//    }
//}
//
//
//ModuleAi::ModuleAi(Application* app, bool start_enabled) : Module(app, start_enabled)
//{
//}
//
//ModuleAi::~ModuleAi()
//{
//}
//
//bool ModuleAi::Start()
//{
//    LOG("Loading AI");
//    // Cargar textura (puedes usar una de color diferente o la misma del player)
//    texture = LoadTexture("Assets-racing/Textures/Car1.png"); // O usa un CarEnemy.png si tienes
//
//    // Esperamos un frame para asegurar que el Mapa ya carg?los puntos en Start()
//    // O mejor, creamos los enemigos aqu?si estamos seguros de que Map->Start() corre antes.
//    // Si no est醩 seguro del orden, llama a CreateEnemy desde ModuleGame::Start.
//
//    return true;
//}
//
//void ModuleAi::CreateEnemy(int startPathIndex)
//{
//    // Verificar que tenemos ruta
//    if (App->map->trackPath.empty()) {
//        LOG("Error: No AI path found in Map");
//        return;
//    }
//
//    // Posici髇 inicial basada en el punto de la ruta
//    b2Vec2 startPos = App->map->trackPath[startPathIndex];
//
//    VehicleInfo info; // Usamos tu struct de info si quieres, o creamos rectangle directo
//
//    // Crear cuerpo f韘ico (Metros a Pixels para la creaci髇, Box2D lo convierte internamente)
//    PhysBody* pb = App->physics->CreateRectangle(
//        METERS_TO_PIXELS(startPos.x),
//        METERS_TO_PIXELS(startPos.y),
//        26, 43,
//        1, 0xFFFF // Ajusta las colisiones si quieres que choquen o no
//    );
//
//    EnemyCar enemy;
//    enemy.Init(pb, startPathIndex);
//    enemies.push_back(enemy);
//}
//
//
//
//
////update_status ModuleAi::Update()
////{
////    if (App->game != nullptr && App->game->game_over == true) return UPDATE_CONTINUE;
////
////    const auto& path = App->map->trackPath;
////    if (path.empty()) return UPDATE_CONTINUE;
////
////    for (auto& car : enemies)
////    {
////        if (!car.active || car.pbody == nullptr) continue;
////
////        b2Body* b = car.pbody->body;
////        b2Vec2 position = b->GetPosition();
////        b2Vec2 target = path[car.currentPathIndex];
////
////        // 1. Calcular vector hacia el objetivo
////        b2Vec2 diff = target - position;
////        float distance = diff.Length();
////
////        // 2. Comprobar si llegamos al punto (Radio un poco m醩 grande para no fallar)
////        if (distance < 4.0f) {
////            car.currentPathIndex++;
////            if (car.currentPathIndex >= path.size()) {
////                car.currentPathIndex = 0;
////            }
////        }
////
////        // --- L覩ICA DE ROTACI覰 F蚐ICA ---
////
////        // 羘gulo deseado hacia el objetivo
////        // Ajuste: Si tu sprite mira hacia ARRIBA, usa + PI/2. Si mira a la DERECHA, no sumes nada.
////        float desiredAngle = atan2f(diff.y, diff.x) + (b2_pi / 2.0f);
////
////        // Calcular la diferencia de 醤gulo m醩 corta (-PI a PI)
////        float currentAngle = b->GetAngle();
////        float nextAngle = desiredAngle - currentAngle;
////        while (nextAngle <= -b2_pi) nextAngle += 2 * b2_pi;
////        while (nextAngle > b2_pi) nextAngle -= 2 * b2_pi;
////
////        // Aplicar giro suave usando velocidad angular (NO SetTransform)
////        // 'turnSpeed' controla qu?tan r醦ido corrige el volante
////        b->SetAngularVelocity(nextAngle * car.turnSpeed);
////
////
////        // --- L覩ICA DE MOVIMIENTO Y DESATASCO ---
////
////        b2Vec2 forwardVec = b2Vec2(sin(b->GetAngle()), -cos(b->GetAngle())); // Sprite UP
////
////        // Detectar si estamos atascados: Queremos ir r醦ido pero la velocidad real es baja
////        float currentSpeed = b->GetLinearVelocity().Length();
////
////        if (currentSpeed < 0.5f && car.stuckTimer < 60) {
////            car.stuckTimer++; // Contamos frames atascado
////        }
////        else if (currentSpeed > 1.0f) {
////            car.stuckTimer = 0; // Reseteamos si nos movemos bien
////        }
////
////        // Si llevamos m醩 de 1 segundo (60 frames) atascados, marcha atr醩
////        if (car.stuckTimer > 60) {
////            // Marcha atr醩 y giro invertido para salir
////            b->ApplyForceToCenter(-0.5f * car.maxSpeed * forwardVec, true);
////            b->SetAngularVelocity(-1.0f); // Girar un poco mientras retrocede
////
////            // Si llevamos mucho tiempo marcha atr醩, intentamos avanzar de nuevo
////            if (car.stuckTimer > 120) car.stuckTimer = 0;
////        }
////        else {
////            // Comportamiento normal: Acelerar hacia adelante
////            // Reducimos la fuerza si el coche tiene que girar mucho (para que no derrape tanto)
////            float turnFactor = 1.0f - (abs(nextAngle) / b2_pi); // 1.0 si recto, 0.0 si giro 180
////            if (turnFactor < 0.0f) turnFactor = 0.0f;
////
////            b2Vec2 finalForce = forwardVec;
////            finalForce *= (car.maxSpeed * turnFactor);
////
////            b->ApplyForceToCenter(finalForce, true);
////        }
////
////        // 5. Eliminar derrape lateral
////        KillOrthogonalVelocity(b);
////    }
////
////    return UPDATE_CONTINUE;
////}
//
//update_status ModuleAi::Update()
//{
//    if (App->game != nullptr && App->game->game_over) return UPDATE_CONTINUE;
//
//    const auto& path = App->map->trackPath;
//    if (path.empty()) return UPDATE_CONTINUE;
//
//    for (auto& car : enemies)
//    {
//        if (!car.active || !car.pbody) continue;
//
//        b2Body* b = car.pbody->body;
//        b2Vec2 position = b->GetPosition();
//        b2Vec2 target = path[car.currentPathIndex];
//
//        // --- 1. SEGUIMIENTO DE RUTA ---
//        b2Vec2 diff = target - position;
//        float distToTarget = diff.Length();
//
//        if (distToTarget < 5.0f) {
//            car.currentPathIndex = (car.currentPathIndex + 1) % path.size();
//        }
//
//        float desiredAngle = atan2f(diff.y, diff.x) + (b2_pi / 2.0f);
//
//        // --- 2. RAYCASTS ---
//        float speed = b->GetLinearVelocity().Length();
//        float sensorLen = 3.0f + (speed * 0.4f);
//
//        // Lanzamos rayos
//        float leftSpace = CastRay(b, sensorLen, -35.0f * DEGTORAD, 0);
//        float rightSpace = CastRay(b, sensorLen, 35.0f * DEGTORAD, 0);
//        float frontSpace = CastRay(b, sensorLen * 1.2f, 0.0f, 1);
//
//        float steeringInfluence = 0.0f;
//        bool avoiding = false;
//
//        // Si hay obst醕ulo a la izquierda, giramos derecha
//        if (leftSpace < 0.9f) {
//            steeringInfluence += (1.0f - leftSpace) * 2.5f;
//            avoiding = true;
//        }
//        // Si hay obst醕ulo a la derecha, giramos izquierda
//        if (rightSpace < 0.9f) {
//            steeringInfluence -= (1.0f - rightSpace) * 2.5f;
//            avoiding = true;
//        }
//
//        // --- 3. DECISI覰 ---
//        float finalAngle = 0.0f;
//
//        if (avoiding) {
//            finalAngle = b->GetAngle() + steeringInfluence;
//            car.maxSpeed = 5.0f;
//        }
//        else {
//            finalAngle = desiredAngle;
//            car.maxSpeed = 10.0f;
//        }
//
//        if (frontSpace < 0.5f) {
//            car.maxSpeed = 2.0f;
//            if (leftSpace > rightSpace) finalAngle = b->GetAngle() - 1.0f;
//            else finalAngle = b->GetAngle() + 1.0f;
//        }
//
//        // --- 4. GIRO ---
//        float currentAngle = b->GetAngle();
//        float nextAngle = finalAngle - currentAngle;
//        while (nextAngle <= -b2_pi) nextAngle += 2 * b2_pi;
//        while (nextAngle > b2_pi) nextAngle -= 2 * b2_pi;
//
//        float turnSpeed = avoiding ? car.turnSpeed * 2.0f : car.turnSpeed;
//        b->SetAngularVelocity(nextAngle * turnSpeed);
//
//        // --- 5. MOVIMIENTO ---
//        // Calcular vector Forward manualmente
//        b2Vec2 forwardVec;
//        forwardVec.x = sin(b->GetAngle());
//        forwardVec.y = -cos(b->GetAngle());
//
//        // Anti-stuck
//        if (speed < 1.0f) car.stuckTimer++;
//        else car.stuckTimer = 0;
//
//        if (car.stuckTimer > 60) {
//            // Marcha atr醩 manual (fuerza negativa)
//            b2Vec2 reverseForce;
//            reverseForce.x = forwardVec.x * -5.0f;
//            reverseForce.y = forwardVec.y * -5.0f;
//
//            b->ApplyForceToCenter(reverseForce, true);
//            b->SetAngularVelocity(1.0f);
//
//            if (car.stuckTimer > 120) car.stuckTimer = 0;
//        }
//        else {
//            // Acelerar hacia adelante si no superamos la velocidad m醲ima
//            // Frenar un poco en curvas cerradas
//            float turnDrag = 1.0f - (abs(nextAngle) / b2_pi);
//            if (turnDrag < 0.2f) turnDrag = 0.2f;
//
//            float currentMaxSpeed = car.maxSpeed * turnDrag;
//
//            if (speed < currentMaxSpeed) {
//                // Aplicar fuerza manualmente
//                b2Vec2 driveForce;
//                driveForce.x = forwardVec.x * 15.0f;
//                driveForce.y = forwardVec.y * 15.0f;
//
//                b->ApplyForceToCenter(driveForce, true);
//            }
//        }
//
//        KillOrthogonalVelocity(b);
//    }
//
//    return UPDATE_CONTINUE;
//}
//
//void ModuleAi::KillOrthogonalVelocity(b2Body* body)
//{
//    b2Vec2 localPoint = b2Vec2(0, 0);
//    b2Vec2 velocity = body->GetLinearVelocityFromLocalPoint(localPoint);
//
//    b2Vec2 sidewaysAxis = body->GetWorldVector(b2Vec2(1, 0)); // Vector derecha local
//    float mag = b2Dot(velocity, sidewaysAxis);
//
//    // Aplicar impulso contrario para anular velocidad lateral
//    b2Vec2 impulse = sidewaysAxis;
//    impulse *= -mag * body->GetMass();
//    body->ApplyLinearImpulse(impulse, body->GetWorldCenter(), true);
//}
//
//update_status ModuleAi::PostUpdate()
//{
//    // Dibujar los coches enemigos
//    for (auto& car : enemies)
//    {
//        if (car.active && car.pbody != nullptr)
//        {
//            int posX, posY;
//            car.pbody->GetPhysicPosition(posX, posY);
//            float rotationDegrees = car.pbody->GetRotation() * RAD2DEG;
//
//            Rectangle sourceRec = { 0.0f, 0.0f, (float)car.width, (float)car.height };
//            Rectangle destRec = { (float)posX, (float)posY, (float)car.width, (float)car.height };
//            Vector2 origin = { (float)car.width / 2, (float)car.height / 2 };
//
//            // Dibujar con tinte rojo para diferenciarlos
//            DrawTexturePro(texture, sourceRec, destRec, origin, rotationDegrees, RED);
//        }
//    }
//
//    // DEBUG: Dibujar la ruta (opcional)
//    if (App->physics->debug && !App->map->trackPath.empty()) {
//        for (const auto& p : App->map->trackPath) {
//            DrawCircle(METERS_TO_PIXELS(p.x), METERS_TO_PIXELS(p.y), 3, GREEN);
//        }
//    }
//
//    return UPDATE_CONTINUE;
//}
//
//bool ModuleAi::CleanUp()
//{
//    UnloadTexture(texture);
//    enemies.clear();
//    return true;
//}
//
//b2Vec2 ModuleAi::CalculateAvoidanceVector(b2Body* body, float rayLength, float angleOffset)
//{
//    b2World* world = body->GetWorld();
//    if (!world) return b2Vec2(0, 0);
//
//    b2Vec2 start = body->GetPosition();
//    float angle = body->GetAngle() + angleOffset;
//    // Asumiendo que tu sprite mira hacia arriba, ajustamos el vector
//    b2Vec2 direction(sin(angle), -cos(angle));
//    b2Vec2 end = start + (rayLength * direction);
//
//    AICallback callback;
//    world->RayCast(&callback, start, end);
//
//    // DEBUG VISUAL
//    if (App->physics->debug) {
//        if (callback.fixture) {
//            DrawLine(METERS_TO_PIXELS(start.x), METERS_TO_PIXELS(start.y),
//                METERS_TO_PIXELS(callback.point.x), METERS_TO_PIXELS(callback.point.y), RED);
//        }
//        else {
//            DrawLine(METERS_TO_PIXELS(start.x), METERS_TO_PIXELS(start.y),
//                METERS_TO_PIXELS(end.x), METERS_TO_PIXELS(end.y), YELLOW);
//        }
//    }
//
//    if (callback.fixture) {
//        // Cuanto m醩 cerca est?la pared, m醩 fuerte es el vector de rechazo
//        float urgency = (1.0f - callback.fraction) * 5.0f; // Multiplicador de fuerza
//
//        // El vector de rechazo es la NORMAL de la pared (perpendicular hacia afuera)
//        return callback.normal * urgency;
//    }
//
//    return b2Vec2(0, 0);
//}//codigo antiguo//codigo antiguo------------------------------------------------------------
//co


///////////
//#include "ModuleAi.h"
//#include "Application.h"
//#include "ModulePhysics.h"
//#include "ModuleMap.h"
//#include "ModuleRender.h"
//#include "ModuleGame.h"
//#include "ModulePlayer.h"
//#include <cmath>
//
//// Definiciones de seguridad por si faltan en tus cabeceras
//#ifndef DEGTORAD
//#define DEGTORAD 0.0174532925199432957f
//#endif
//#ifndef RAD2DEG
//#define RAD2DEG 57.295779513082320876f
//#endif
//
//// Clase para gestionar los impactos del Raycast
//class AICallback : public b2RayCastCallback {
//public:
//    b2Fixture* fixture = nullptr;
//    b2Vec2 point;
//    b2Vec2 normal;
//    float fraction = 1.0f;
//
//    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
//        if (fixture->IsSensor()) return -1.0f; // Ignorar sensores/checkpoints
//        this->fixture = fixture;
//        this->point = point;
//        this->normal = normal;
//        this->fraction = fraction;
//        return fraction;
//    }
//};
//
//void EnemyCar::Init(PhysBody* body, int startPathIndex) {
//    pbody = body;
//    currentPathIndex = startPathIndex;
//    active = true;
//    if (pbody && pbody->body) {
//        pbody->body->SetLinearDamping(0.5f);
//        pbody->body->SetAngularDamping(2.0f);
//    }
//}
//
////void EnemyCar::Init(PhysBody* body, int startPathIndex) {
////    pbody = body;
////    currentPathIndex = startPathIndex;
////    active = true;
////    if (pbody && pbody->body) {
////        // Valores ajustados para estabilidad
////        pbody->body->SetLinearDamping(0.5f);
////        pbody->body->SetAngularDamping(3.0f);
////
////        // Aumentamos la masa o fricci髇 si es necesario para que no salga volando al chocar
////        pbody->body->GetFixtureList()->SetRestitution(0.1f); // Poco rebote
////    }
////}
//
//ModuleAi::ModuleAi(Application* app, bool start_enabled) : Module(app, start_enabled) {}
//ModuleAi::~ModuleAi() {}
//
//bool ModuleAi::Start()
//{
//    LOG("Loading AI");
//    texture = LoadTexture("Assets-racing/Textures/Car1.png");
//    return true;
//}
//
//void ModuleAi::CreateEnemy(int startPathIndex)
//{
//    if (App->map->trackPath.empty()) return;
//
//    b2Vec2 startPos = App->map->trackPath[startPathIndex];
//
//    PhysBody* pb = App->physics->CreateRectangle(
//        METERS_TO_PIXELS(startPos.x),
//        METERS_TO_PIXELS(startPos.y),
//        26, 43,
//        1, 0xFFFF
//    );
//
//    EnemyCar enemy;
//    enemy.Init(pb, startPathIndex);
//    enemies.push_back(enemy);
//}
//
//// Implementaci髇 CORRECTA de CastRay (sin usar operadores prohibidos)
//float ModuleAi::CastRay(b2Body* body, float rayLength, float angleOffset, int colorType)
//{
//    b2World* world = body->GetWorld();
//    if (!world) return 1.0f;
//
//    b2Vec2 start = body->GetPosition();
//    float angle = body->GetAngle() + angleOffset;
//
//    // C醠culo manual de direcci髇 (sin vector * float)
//    b2Vec2 direction;
//    direction.x = sin(angle);
//    direction.y = -cos(angle);
//
//    // C醠culo manual del punto final
//    b2Vec2 end;
//    end.x = start.x + (direction.x * rayLength);
//    end.y = start.y + (direction.y * rayLength);
//
//    AICallback callback;
//    world->RayCast(&callback, start, end);
//
//    // Dibujado Debug
//    if (App->physics->debug) {
//        Color c = (colorType == 0) ? YELLOW : ORANGE;
//        if (callback.fixture) c = RED;
//
//        // Determinar punto final visual
//        b2Vec2 drawEnd = (callback.fixture) ? callback.point : end;
//
//        DrawLine(METERS_TO_PIXELS(start.x), METERS_TO_PIXELS(start.y),
//            METERS_TO_PIXELS(drawEnd.x), METERS_TO_PIXELS(drawEnd.y), c);
//    }
//
//    return callback.fraction;
//}
//
////update_status ModuleAi::Update()
////{
////    if (App->game != nullptr && App->game->game_over) return UPDATE_CONTINUE;
////
////    const auto& path = App->map->trackPath;
////    if (path.empty()) return UPDATE_CONTINUE;
////
////    for (auto& car : enemies)
////    {
////        if (!car.active || !car.pbody) continue;
////
////        b2Body* b = car.pbody->body;
////        b2Vec2 position = b->GetPosition();
////        b2Vec2 target = path[car.currentPathIndex];
////
////        // --- 1. SEGUIMIENTO DE RUTA ---
////        b2Vec2 diff = target - position;
////        float distToTarget = diff.Length();
////
////        if (distToTarget < 5.0f) {
////            car.currentPathIndex = (car.currentPathIndex + 1) % path.size();
////        }
////
////        float desiredAngle = atan2f(diff.y, diff.x) + (b2_pi / 2.0f);
////
////        // --- 2. RAYCASTS ---
////        float speed = b->GetLinearVelocity().Length();
////        float sensorLen = 3.0f + (speed * 0.4f);
////
////        // Llamamos a CastRay (ahora s?existe)
////        float leftSpace = CastRay(b, sensorLen, -35.0f * DEGTORAD, 0);
////        float rightSpace = CastRay(b, sensorLen, 35.0f * DEGTORAD, 0);
////        float frontSpace = CastRay(b, sensorLen * 1.2f, 0.0f, 1);
////
////        float steeringInfluence = 0.0f;
////        bool avoiding = false;
////
////        if (leftSpace < 0.9f) {
////            steeringInfluence += (1.0f - leftSpace) * 2.5f;
////            avoiding = true;
////        }
////        if (rightSpace < 0.9f) {
////            steeringInfluence -= (1.0f - rightSpace) * 2.5f;
////            avoiding = true;
////        }
////
////        // --- 3. DECISI覰 ---
////        float finalAngle = 0.0f;
////
////        if (avoiding) {
////            finalAngle = b->GetAngle() + steeringInfluence;
////            car.maxSpeed = 5.0f;
////        }
////        else {
////            finalAngle = desiredAngle;
////            car.maxSpeed = 10.0f;
////        }
////
////        if (frontSpace < 0.5f) {
////            car.maxSpeed = 2.0f;
////            if (leftSpace > rightSpace) finalAngle = b->GetAngle() - 1.0f;
////            else finalAngle = b->GetAngle() + 1.0f;
////        }
////
////        // --- 4. GIRO ---
////        float currentAngle = b->GetAngle();
////        float nextAngle = finalAngle - currentAngle;
////        while (nextAngle <= -b2_pi) nextAngle += 2 * b2_pi;
////        while (nextAngle > b2_pi) nextAngle -= 2 * b2_pi;
////
////        float turnSpeed = avoiding ? car.turnSpeed * 2.0f : car.turnSpeed;
////        b->SetAngularVelocity(nextAngle * turnSpeed);
////
////        // --- 5. MOVIMIENTO ---
////        b2Vec2 forwardVec;
////        forwardVec.x = sin(b->GetAngle());
////        forwardVec.y = -cos(b->GetAngle());
////
////        if (speed < 1.0f) car.stuckTimer++;
////        else car.stuckTimer = 0;
////
////        if (car.stuckTimer > 60) {
////            // Marcha atr醩 manual
////            b2Vec2 reverseForce;
////            reverseForce.x = forwardVec.x * -5.0f;
////            reverseForce.y = forwardVec.y * -5.0f;
////
////            b->ApplyForceToCenter(reverseForce, true);
////            b->SetAngularVelocity(1.0f);
////
////            if (car.stuckTimer > 120) car.stuckTimer = 0;
////        }
////        else {
////            float turnDrag = 1.0f - (abs(nextAngle) / b2_pi);
////            if (turnDrag < 0.2f) turnDrag = 0.2f;
////
////            float currentMaxSpeed = car.maxSpeed * turnDrag;
////
////            if (speed < currentMaxSpeed) {
////                b2Vec2 driveForce;
////                driveForce.x = forwardVec.x * 15.0f;
////                driveForce.y = forwardVec.y * 15.0f;
////
////                b->ApplyForceToCenter(driveForce, true);
////            }
////        }
////
////        KillOrthogonalVelocity(b);
////    }
////
////    return UPDATE_CONTINUE;
////}
//
//update_status ModuleAi::Update()
//{
//    if (App->game != nullptr && App->game->game_over) return UPDATE_CONTINUE;
//
//    const auto& path = App->map->trackPath;
//    if (path.empty()) return UPDATE_CONTINUE;
//
//    for (auto& car : enemies)
//    {
//        if (!car.active || !car.pbody) continue;
//
//        b2Body* b = car.pbody->body;
//        b2Vec2 position = b->GetPosition();
//
//        // --- 1. SEGUIMIENTO DE RUTA ROBUSTO ---
//
//        // Buscar el punto m醩 cercano en toda la ruta (para no perderse nunca)
//        // Optimizamos buscando solo en los siguientes 5 puntos para no iterar todo siempre
//        int bestIndex = car.currentPathIndex;
//        float minDistance = 100000.0f;
//
//        for (int i = 0; i < 5; i++) {
//            int idx = (car.currentPathIndex + i) % path.size();
//            b2Vec2 diff = path[idx] - position;
//            float dist = diff.Length();
//            if (dist < minDistance) {
//                minDistance = dist;
//                bestIndex = idx;
//            }
//        }
//        car.currentPathIndex = bestIndex;
//
//        // TARGET: Miramos 2 puntos por delante para cortar curvas suavemente
//        int targetIndex = (car.currentPathIndex + 2) % path.size();
//        b2Vec2 target = path[targetIndex];
//
//        b2Vec2 diff = target - position;
//        float desiredAngle = atan2f(diff.y, diff.x) + (b2_pi / 2.0f);
//
//        // --- 2. EVASI覰 SUAVE ---
//        float speed = b->GetLinearVelocity().Length();
//        float lookAhead = 4.0f; // Rayos fijos para estabilidad
//
//        float leftSpace = CastRay(b, lookAhead, -30.0f * DEGTORAD, 0);
//        float rightSpace = CastRay(b, lookAhead, 30.0f * DEGTORAD, 0);
//        float frontSpace = CastRay(b, lookAhead * 1.2f, 0.0f, 1);
//
//        float finalAngle = desiredAngle;
//        float avoidanceFactor = 0.0f;
//
//        // Si detectamos pared, mezclamos el 醤gulo deseado con la correcci髇
//        // No reemplazamos totalmente el 醤gulo, solo lo empujamos
//        if (leftSpace < 0.8f) {
//            avoidanceFactor += (0.8f - leftSpace) * 2.0f; // Empujar derecha
//        }
//        if (rightSpace < 0.8f) {
//            avoidanceFactor -= (0.8f - rightSpace) * 2.0f; // Empujar izquierda
//        }
//
//        finalAngle += avoidanceFactor;
//
//        // --- 3. GIRO ---
//        float currentAngle = b->GetAngle();
//        float nextAngle = finalAngle - currentAngle;
//        while (nextAngle <= -b2_pi) nextAngle += 2 * b2_pi;
//        while (nextAngle > b2_pi) nextAngle -= 2 * b2_pi;
//
//        // Giramos con Clamp para evitar que el volante se vuelva loco
//        float turnSpeed = 3.0f;
//        b->SetAngularVelocity(nextAngle * turnSpeed);
//
//        // --- 4. MOVIMIENTO (CORREGIDO PARA CURVAS) ---
//        b2Vec2 forwardVec;
//        forwardVec.x = sin(b->GetAngle());
//        forwardVec.y = -cos(b->GetAngle());
//
//        // --- L骻ica Anti-Atasco (Stuck) MENOS SENSIBLE ---
//        // Solo activamos si la velocidad es casi CERO (0.2)
//        if (speed < 0.2f) car.stuckTimer++;
//        else car.stuckTimer = 0;
//
//        if (car.stuckTimer > 60) {
//            // Maniobra de rescate
//            b2Vec2 reverseForce;
//            reverseForce.x = forwardVec.x * -4.0f;
//            reverseForce.y = forwardVec.y * -4.0f;
//            b->ApplyForceToCenter(reverseForce, true);
//
//            // Girar mientras retrocede para desencajarse
//            b->SetAngularVelocity(1.5f);
//
//            if (car.stuckTimer > 120) car.stuckTimer = 0;
//        }
//        else {
//            // CONDUCCI覰 NORMAL
//
//            // Frenado en curvas: Mucho m醩 permisivo.
//            // Antes frenaba si el 醤gulo era > 0.2, ahora permitimos hasta 0.5 sin frenar apenas
//            float turnDrag = 1.0f;
//            if (abs(nextAngle) > 0.5f) {
//                turnDrag = 0.6f; // Reducimos velocidad al 60% solo en curvas cerradas
//            }
//
//            // Si hay pared enfrente, frenamos m醩
//            if (frontSpace < 0.6f) turnDrag *= 0.5f;
//
//            float currentMaxSpeed = car.maxSpeed * turnDrag;
//
//            // Aceleramos
//            if (speed < currentMaxSpeed) {
//                b2Vec2 driveForce;
//                driveForce.x = forwardVec.x * 12.0f;
//                driveForce.y = forwardVec.y * 12.0f;
//                b->ApplyForceToCenter(driveForce, true);
//            }
//        }
//
//        KillOrthogonalVelocity(b);
//    }
//
//    return UPDATE_CONTINUE;
//}
//
//
//update_status ModuleAi::PostUpdate()
//{
//    for (auto& car : enemies)
//    {
//        if (car.active && car.pbody != nullptr)
//        {
//            int posX, posY;
//            car.pbody->GetPhysicPosition(posX, posY);
//            float rotationDegrees = car.pbody->GetRotation() * RAD2DEG;
//
//            Rectangle sourceRec = { 0.0f, 0.0f, (float)car.width, (float)car.height };
//            Rectangle destRec = { (float)posX, (float)posY, (float)car.width, (float)car.height };
//            Vector2 origin = { (float)car.width / 2, (float)car.height / 2 };
//
//            DrawTexturePro(texture, sourceRec, destRec, origin, rotationDegrees, RED);
//        }
//    }
//
//    if (App->physics->debug && !App->map->trackPath.empty()) {
//        for (const auto& p : App->map->trackPath) {
//            DrawCircle(METERS_TO_PIXELS(p.x), METERS_TO_PIXELS(p.y), 3, GREEN);
//        }
//    }
//    return UPDATE_CONTINUE;
//}
//
//bool ModuleAi::CleanUp()
//{
//    UnloadTexture(texture);
//    enemies.clear();
//    return true;
//}

#include "ModuleAi.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleMap.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModulePlayer.h"
#include "Application.h"
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
    //if (App->map->trackPath.empty()) return;

    int randomPathID = rand() % App->map->trackPaths.size();
    if (App->map->trackPaths[randomPathID].empty())return;

    b2Vec2 startPos = App->map->trackPaths[randomPathID][startPathIndex];

    PhysBody* pb = App->physics->CreateRectangle(
        METERS_TO_PIXELS(startPos.x),
        METERS_TO_PIXELS(startPos.y),
        26, 43,
        1, 0xFFFF
    );

    EnemyCar enemy;
    enemy.Init(pb, startPathIndex,randomPathID);
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

        //if (frontSpace < 0.5f) {
        //    if (hitPlayerFront) {
        //        // 如果是玩家，可能想撞过去，或者稍微减速但保持攻击性
        //        car.maxSpeed = 8.0f;
        //        // AI 可能会尝试轻微变道去超车，而不是像躲墙一样剧烈转向
        //    }
        //    else {
        //        // 如果是墙壁，必须大力刹车和转向
        //        car.maxSpeed = 2.0f;
        //    }
        //}

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
            // CONDUCCI覰 NORMAL

            // Frenado en curvas: Mucho m醩 permisivo.
            // Antes frenaba si el 醤gulo era > 0.2, ahora permitimos hasta 0.5 sin frenar apenas
            //float turnDrag = 1.0f;
            //if (abs(nextAngle) > 0.5f) {
            //    turnDrag = 0.6f; // Reducimos velocidad al 60% solo en curvas cerradas
            //}

            //// Si hay pared enfrente, frenamos m醩
            //if (frontSpace < 0.6f) turnDrag *= 0.5f;

            //float currentMaxSpeed = car.maxSpeed * turnDrag;

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
    b2Vec2 localPoint(0, 0);
    b2Vec2 velocity = body->GetLinearVelocityFromLocalPoint(localPoint);
    b2Vec2 sidewaysAxis = body->GetWorldVector(b2Vec2(1, 0));
    float mag = b2Dot(velocity, sidewaysAxis);

    // Reducimos la deriva lateral al 95% en cada frame (derrape controlado)
    // Usar 1.0f quita el derrape totalmente (coche sobre ra韑es)
    // Usar 0.9f permite derrape
    float driftCorrection = 0.95f;

    b2Vec2 impulse;
    impulse.x = sidewaysAxis.x * (-mag * body->GetMass() * driftCorrection);
    impulse.y = sidewaysAxis.y * (-mag * body->GetMass() * driftCorrection);
    body->ApplyLinearImpulse(impulse, body->GetWorldCenter(), true);
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

            //if (App->physics->debug && !App->map->trackPath.empty()) {
            //    b2Vec2 t = App->map->trackPath[(car.currentPathIndex + 2) % App->map->trackPath.size()];
            //    DrawLine(posX, posY, METERS_TO_PIXELS(t.x), METERS_TO_PIXELS(t.y), BLUE);
            //}
       /*     if (App->physics->debug) {
                for (const auto& path : App->map->trackPaths) {
                    for (const auto& p : path) {
                        DrawCircle(METERS_TO_PIXELS(p.x), METERS_TO_PIXELS(p.y), 3, GREEN);
                    }
                }
            }*/
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

bool ModuleAi::CleanUp()
{
    UnloadTexture(texture);
    enemies.clear();
    return true;
}