#include "Globals.h"
#include "Application.h"
#include "ModuleAi.h"
#include "ModulePhysics.h"
#include <cmath>

ModuleAi::ModuleAi(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    pbody = nullptr;
    currentPathIndex = 0;
}

ModuleAi::~ModuleAi() {}

bool ModuleAi::Start()
{
    LOG("Loading Ai");

    // 1. Cargar Textura (puedes usar un sprite distinto si quieres)
    texture = LoadTexture("Assets-racing/Textures/Car1.png"); // O usa 'enemy_spritesheet.png'

    // 2. Definir Stats (IDENTICAS al Player para competencia justa)
    info.width = 26;
    info.height = 43;
    info.acceleration = 2.0f;
    info.maxSpeed = 10.0f;
    info.turnSpeed = 3.0f;

    // 3. Crear Físicas
    // Posición inicial desplazada para no empezar encima del jugador
    pbody = App->physics->CreateRacingCar(100 + 50, 300, info);
    pbody->listener = this;

    // 4. Cargar el camino (Copiado de Coords_Mid de tu ModuleMap)
    LoadTrackPoints();

    return true;
}

update_status ModuleAi::Update()
{
    if (pbody != nullptr)
    {
        HandleMovement();
    }
    return UPDATE_CONTINUE;
}

void ModuleAi::HandleMovement()
{
    b2Body* b = pbody->body;

    // 1. Obtener posición actual
    b2Vec2 pos = b->GetPosition(); // En metros
    iPoint pixelPos(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y));

    // 2. Comprobar waypoint objetivo
    if (pathPoints.empty()) return;

    iPoint target = pathPoints[currentPathIndex];

    // Distancia al objetivo
    float dx = (float)(target.x - pixelPos.x);
    float dy = (float)(target.y - pixelPos.y);
    float dist = sqrt(dx * dx + dy * dy);

    // Si estamos cerca, pasar al siguiente punto
    if (dist < 50.0f) { // Radio de aceptación de 50 pixeles
        currentPathIndex++;
        if (currentPathIndex >= pathPoints.size()) currentPathIndex = 0; // Vuelta al inicio
        return;
    }

    // 3. Calcular Ángulo Deseado
    // atan2 devuelve el ángulo en radianes hacia el objetivo
    float targetAngle = atan2(dy, dx);

    // El ángulo actual de Box2D (ajustado porque atan2 es matemático estándar y Box2D/Raylib pueden variar)
    // Asumiendo que el coche mira hacia la derecha en 0 grados.
    // Si tu sprite mira hacia arriba, añade -PI/2 al targetAngle.
    float currentAngle = b->GetAngle();

    // Normalizar ángulo actual a rango -PI a PI para comparar
    while (currentAngle <= -PI) currentAngle += 2 * PI;
    while (currentAngle > PI) currentAngle -= 2 * PI;

    // Calcular diferencia de giro necesaria
    float angleDiff = targetAngle - currentAngle;
    // Ajuste para el camino más corto de giro
    while (angleDiff <= -PI) angleDiff += 2 * PI;
    while (angleDiff > PI) angleDiff -= 2 * PI;

    // 4. Aplicar Inputs (Simular teclas)
    b2Vec2 velocity = b->GetLinearVelocity();
    float currentSpeed = velocity.Length();

    // -- GIRAR --
    float turnThreshold = 0.1f; // Pequeña zona muerta
    if (angleDiff > turnThreshold) {
        b->SetAngularVelocity(info.turnSpeed); // Derecha
    }
    else if (angleDiff < -turnThreshold) {
        b->SetAngularVelocity(-info.turnSpeed); // Izquierda
    }
    else {
        b->SetAngularVelocity(0.0f); // Ir recto
    }

    // Reducir velocidad al girar (igual que player)
    if (abs(b->GetAngularVelocity()) > 0.1f) {
        velocity.x *= info.turnDrag;
        velocity.y *= info.turnDrag;
        b->SetLinearVelocity(velocity);
    }

    // -- ACELERAR --
    // Solo acelerar si el coche está más o menos encarado al objetivo (90 grados)
    if (abs(angleDiff) < PI / 2)
    {
        if (currentSpeed < info.maxSpeed) {
            b2Vec2 direction(cos(currentAngle), sin(currentAngle)); // Asume coche mira a derecha (0 rad)
            // Si el sprite mira arriba usar: b2Vec2 direction(sin(currentAngle), -cos(currentAngle));

            direction *= info.acceleration;
            b->ApplyForceToCenter(direction, true);
        }
    }

    // -- FRICCIÓN BASE -- (Igual que Player)
    velocity = b->GetLinearVelocity();
    velocity.x *= info.friction;
    velocity.y *= info.friction;
    b->SetLinearVelocity(velocity);
}

update_status ModuleAi::PostUpdate()
{
    if (pbody != nullptr)
    {
        int posX, posY;
        pbody->GetPhysicPosition(posX, posY);
        float rotationDegrees = pbody->GetRotation() * RAD2DEG;

        Rectangle sourceRec = { 0.0f, 0.0f, (float)info.width, (float)info.height };
        Rectangle destRec = { (float)posX, (float)posY, (float)info.width, (float)info.height };
        Vector2 origin = { (float)info.width / 2, (float)info.height / 2 };

        // Pinta el enemigo de rojo para diferenciarlo
        DrawTexturePro(texture, sourceRec, destRec, origin, rotationDegrees, RED);
    }
    return UPDATE_CONTINUE;
}

bool ModuleAi::CleanUp()
{
    LOG("Unloading Enemy");
    UnloadTexture(texture);
    return true;
}

void ModuleAi::LoadTrackPoints()
{
    // Estos son los puntos de Coords_Mid copiados de ModuleMap.cpp
    // Son enteros x, y
    int rawPoints[] = {
        720, 746, 1604, 747, 1627, 743, 1655, 736, 1680, 719, 1696, 695, 1706, 667,
        1708, 639, 1707, 511, 1705, 483, 1695, 456, 1680, 431, 1655, 415, 1626, 405,
        1602, 404, 831, 404, 816, 398, 811, 383, 817, 368, 831, 363, 1760, 364,
        1788, 361, 1815, 352, 1840, 336, 1856, 311, 1865, 281, 1868, 256, 1867, 126,
        1864, 100, 1855, 72, 1840, 47, 1815, 31, 1788, 23, 1763, 21, 640, 20,
        611, 22, 585, 32, 560, 48, 543, 72, 534, 98, 531, 127, 531, 388, 525, 401,
        510, 403, 496, 398, 491, 387, 491, 128, 490, 99, 481, 72, 462, 48, 439, 30,
        412, 23, 383, 19, 126, 20, 100, 23, 72, 32, 47, 50, 32, 71, 22, 99, 19, 125,
        18, 640, 22, 668, 32, 695, 47, 720, 72, 736, 99, 745, 129, 747, 703, 747
    };

    int count = sizeof(rawPoints) / sizeof(rawPoints[0]);
    for (int i = 0; i < count; i += 2)
    {
        pathPoints.push_back({ rawPoints[i], rawPoints[i + 1] });
    }
}