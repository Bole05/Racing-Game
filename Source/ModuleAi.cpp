#include "ModuleAi.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleMap.h"
#include "ModuleRender.h"

void EnemyCar::Init(PhysBody* body, int startPathIndex) {
    pbody = body;
    currentPathIndex = startPathIndex;
    active = true;

    // Como hemos incluido ModulePhysics.h, ahora sí podemos acceder a 'body'
    if (pbody != nullptr && pbody->body != nullptr) {
        pbody->body->SetLinearDamping(0.2f);
        pbody->body->SetAngularDamping(2.0f);
    }
}


ModuleAi::ModuleAi(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleAi::~ModuleAi()
{
}

bool ModuleAi::Start()
{
    LOG("Loading AI");
    // Cargar textura (puedes usar una de color diferente o la misma del player)
    texture = LoadTexture("Assets-racing/Textures/Car1.png"); // O usa un CarEnemy.png si tienes

    // Esperamos un frame para asegurar que el Mapa ya cargó los puntos en Start()
    // O mejor, creamos los enemigos aquí si estamos seguros de que Map->Start() corre antes.
    // Si no estás seguro del orden, llama a CreateEnemy desde ModuleGame::Start.

    return true;
}

void ModuleAi::CreateEnemy(int startPathIndex)
{
    // Verificar que tenemos ruta
    if (App->map->trackPath.empty()) {
        LOG("Error: No AI path found in Map");
        return;
    }

    // Posición inicial basada en el punto de la ruta
    b2Vec2 startPos = App->map->trackPath[startPathIndex];

    VehicleInfo info; // Usamos tu struct de info si quieres, o creamos rectangle directo

    // Crear cuerpo físico (Metros a Pixels para la creación, Box2D lo convierte internamente)
    PhysBody* pb = App->physics->CreateRectangle(
        METERS_TO_PIXELS(startPos.x),
        METERS_TO_PIXELS(startPos.y),
        26, 43,
        1, 0xFFFF // Ajusta las colisiones si quieres que choquen o no
    );

    EnemyCar enemy;
    enemy.Init(pb, startPathIndex);
    enemies.push_back(enemy);
}

update_status ModuleAi::Update()
{
    // Si no hay ruta, no hacemos nada
    const auto& path = App->map->trackPath;
    if (path.empty()) return UPDATE_CONTINUE;

    for (auto& car : enemies)
    {
        if (!car.active || car.pbody == nullptr) continue;

        b2Body* b = car.pbody->body;
        b2Vec2 position = b->GetPosition();
        b2Vec2 target = path[car.currentPathIndex];

        // 1. Calcular distancia al siguiente punto
        b2Vec2 diff = target - position;
        float distance = diff.Length();

        // 2. Si estamos cerca (ej: 3 metros), cambiar al siguiente punto
        if (distance < 3.0f) {
            car.currentPathIndex++;
            if (car.currentPathIndex >= path.size()) {
                car.currentPathIndex = 0; // Loop al inicio
            }
        }

        // 3. STEERING: Girar hacia el objetivo
        // Ángulo deseado (atan2 devuelve radianes)
        // Ajustamos +90 grados (PI/2) o -90 dependiendo de hacia donde mire tu sprite original.
        // Asumimos que el sprite mira hacia ARRIBA. Box2D 0 grados es DERECHA.
        // Si sprite mira ARRIBA: atan2(y,x) - PI/2
        float desiredAngle = atan2f(diff.y, diff.x) + (b2_pi / 2.0f);

        float currentAngle = b->GetAngle();
        float nextAngle = currentAngle + car.turnSpeed * (desiredAngle - currentAngle) * (1.0f / 60.0f);

        // Manera simple de girar (SetTransform). Para física real usaríamos Torque.
        // Para evitar rotaciones bruscas de 360 a 0, se suele normalizar el ángulo, 
        // pero para empezar, SetTransform directo al desiredAngle funciona bien en arcade.
        b->SetTransform(position, desiredAngle);

        // 4. ACELERAR
        // Vector hacia adelante según el ángulo del coche
        b2Vec2 forwardVec = b2Vec2(sin(desiredAngle), -cos(desiredAngle)); // Ajustado para Sprite UP
        // Si usas Sprite RIGHT sería: (cos, sin)

        forwardVec *= car.maxSpeed;
        b->ApplyForceToCenter(forwardVec, true);

        // 5. Eliminar derrape lateral (Física)
        KillOrthogonalVelocity(b);
    }

    return UPDATE_CONTINUE;
}

void ModuleAi::KillOrthogonalVelocity(b2Body* body)
{
    b2Vec2 localPoint = b2Vec2(0, 0);
    b2Vec2 velocity = body->GetLinearVelocityFromLocalPoint(localPoint);

    b2Vec2 sidewaysAxis = body->GetWorldVector(b2Vec2(1, 0)); // Vector derecha local
    float mag = b2Dot(velocity, sidewaysAxis);

    // Aplicar impulso contrario para anular velocidad lateral
    b2Vec2 impulse = sidewaysAxis;
    impulse *= -mag * body->GetMass();
    body->ApplyLinearImpulse(impulse, body->GetWorldCenter(), true);
}

update_status ModuleAi::PostUpdate()
{
    // Dibujar los coches enemigos
    for (auto& car : enemies)
    {
        if (car.active && car.pbody != nullptr)
        {
            int posX, posY;
            car.pbody->GetPhysicPosition(posX, posY);
            float rotationDegrees = car.pbody->GetRotation() * RAD2DEG;

            Rectangle sourceRec = { 0.0f, 0.0f, (float)car.width, (float)car.height };
            Rectangle destRec = { (float)posX, (float)posY, (float)car.width, (float)car.height };
            Vector2 origin = { (float)car.width / 2, (float)car.height / 2 };

            // Dibujar con tinte rojo para diferenciarlos
            DrawTexturePro(texture, sourceRec, destRec, origin, rotationDegrees, RED);
        }
    }

    // DEBUG: Dibujar la ruta (opcional)
    if (App->physics->debug && !App->map->trackPath.empty()) {
        for (const auto& p : App->map->trackPath) {
            DrawCircle(METERS_TO_PIXELS(p.x), METERS_TO_PIXELS(p.y), 3, GREEN);
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