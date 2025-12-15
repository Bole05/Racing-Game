//#pragma once
//#include "Module.h"
//#include "Globals.h"
//#include "p2Point.h"
//#include "box2d/box2d.h"
//#include <vector>
//
//class PhysBody;
//
//struct EnemyCar {
//    PhysBody* pbody = nullptr;
//    int currentPathIndex = 0;
//
//    // Propiedades del coche
//    float maxSpeed = 4.0f;      // Un poco más lento que el jugador (que es 10.0f)
//    float turnSpeed = 2.5f;     // Velocidad de giro
//    int width = 26;
//    int height = 43;
//    bool active = false;
//    int stuckTimer = 0;
//    // Métodos helper
//    void Init(PhysBody* body, int startPathIndex);
//};
//
//class ModuleAi : public Module
//{
//public:
//    ModuleAi(Application* app, bool start_enabled = true);
//    virtual ~ModuleAi();
//
//    bool Start() override;
//    update_status Update() override;
//    update_status PostUpdate() override;
//    bool CleanUp() override;
//
//    // Función para crear un enemigo
//    void CreateEnemy(int pathIndexOffset = 0);
//
//public:
//    Texture2D texture;
//    std::vector<EnemyCar> enemies;
//
//private:
//    // Física auxiliar para evitar derrapes infinitos
//    void KillOrthogonalVelocity(b2Body* body);
//    b2Vec2 CalculateAvoidanceVector(b2Body* body, float rayLength, float angleOffset);
//};//codigo antiguo---------------------------------------------------------------------



#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "box2d/box2d.h"
#include <vector>

class PhysBody;

struct EnemyCar {
    PhysBody* pbody = nullptr;
    int currentPathIndex = 0;
    int selectedPathIndex = 0;
    // Propiedades del coche
    float maxSpeed = 10.0f;
    float turnSpeed = 4.0f;
    int width = 26;
    int height = 43;
    bool active = false;
    int stuckTimer = 0;

    // Métodos helper
    void Init(PhysBody* body, int startPathIndex,int pathIndex);
};

class ModuleAi : public Module
{
public:
    ModuleAi(Application* app, bool start_enabled = true);
    virtual ~ModuleAi();

    bool Start() override;
    update_status Update() override;
    update_status PostUpdate() override;
    bool CleanUp() override;

    // Función para crear un enemigo
    void CreateEnemy(int pathIndexOffset = 0);

public:
    Texture2D texture;
    std::vector<EnemyCar> enemies;

private:
    // Física auxiliar para evitar derrapes infinitos
    void KillOrthogonalVelocity(b2Body* body);

    // --- ESTA ES LA LÍNEA QUE TE FALTABA ---
    // Función auxiliar para lanzar rayos (retorna distancia 0.0 a 1.0)
    float CastRay(b2Body* body, float rayLength, float angleOffset, int colorType);
};