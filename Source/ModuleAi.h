#pragma once
#inlude"Module.h"
#include "p2Point.h"
#include "ModulePhysics.h"
class ModuleAi : public Module
{
public:
    ModuleAi(Application* app, bool start_enabled = true);
    virtual ~ModuleAi();

    bool Start();
    update_status Update(float dt);
    bool CleanUp();

public:
    PhysBody* vehicle; // El cuerpo físico del enemigo
    VehicleInfo stats; // Sus estadísticas (idénticas al player)

    // Variables para la lógica de conducción
    float currentTurn;
    float currentAcceleration;
};