#pragma once
#include "Module.h"
#include "p2Point.h"
#include "ModulePhysics.h"

class ModuleAi : public Module
{
public:
    ModuleAi(Application* app, bool start_enabled = true);
    virtual ~ModuleAi();

    bool Start();
    update_status Update();
    update_status PostUpdate();
    bool CleanUp();

public:
    PhysBody* pbody;
    VehicleInfo info; // Aquí guardaremos las mismas stats que el player
    Texture2D texture;

    // IA Variables
    std::vector<iPoint> pathPoints; // Puntos de ruta
    int currentPathIndex;

private:
    void LoadTrackPoints(); // Carga los puntos del mapa
    void HandleMovement();  // Lógica de conducción automática
};