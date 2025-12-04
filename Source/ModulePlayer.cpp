#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
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
	texture = LoadTexture("Assets-racing/Textures/Car.png");
	
   /* int frameWidth = texture.width / 4;*/
    int frameWidth = 64;
    int frameHeight = 128;
    x = 100;
    y = 300;
    pbody = App->physics->CreateRectangle(x, y, frameWidth, frameHeight, 1,0);

    this->width = frameWidth;
    this->height = frameHeight;
	if (pbody != nullptr)
	{
		// Damping: "Freno" natural. Si sueltas el gas, el coche para.
		pbody->body->SetLinearDamping(1.0f);  // Fricción de movimiento
		pbody->body->SetAngularDamping(2.0f); // Fricción de rotación
	}

	// Ajustar variables de velocidad
	speed = 15.0f;       // Fuerza de aceleración
	turn_speed = 3.0f;   // Velocidad de giro

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
    UnloadTexture(texture);
	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
    if (pbody != nullptr)
    {
        b2Body* b = pbody->body; // Acceso directo a Box2D

        // Girar (A/D o Izquierda/Derecha) -> Aplica Torque o velocidad angular
        if (IsKeyDown(KEY_LEFT)) {
            // Opción A: Torque (más realista, con inercia)
            // b->ApplyTorque(-turn_speed, true);
            // Opción B: Velocidad directa (más arcade/preciso)
            b->SetAngularVelocity(-turn_speed);
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            b->SetAngularVelocity(turn_speed);
        }
        else {
            // Si no tocas nada, deja de girar (útil para control arcade)
            b->SetAngularVelocity(0.0f);
        }

        // Acelerar (W o Arriba) -> Aplica fuerza en la dirección que mira el coche
        if (IsKeyDown(KEY_UP))
        {
            // Obtener el vector "hacia adelante" basado en el ángulo actual
            // En Box2D el ángulo 0 suele ser hacia la derecha. Depende de tu sprite.
            // Asumiendo que el sprite mira hacia arriba o derecha, ajusta el seno/coseno.

            // Si el sprite mira a la DERECHA por defecto:
            float angle = b->GetAngle();
            b2Vec2 direction(cos(angle), sin(angle));

            // Si el sprite mira hacia ARRIBA por defecto, usa:
            // b2Vec2 direction(sin(angle), -cos(angle)); // Ajustar signos según coordenadas

            // Escalar el vector por la velocidad
            direction *= speed;

            // Aplicar fuerza en el centro de masa
            b->ApplyForceToCenter(direction, true);
        }

        // Frenar / Marcha atrás
        if (IsKeyDown(KEY_DOWN))
        {
            float angle = b->GetAngle();
            b2Vec2 direction(cos(angle), sin(angle));
            direction *= -speed * 0.5f; // Marcha atrás más lenta
            b->ApplyForceToCenter(direction, true);
        }
    

    // --- RENDERIZADO (RAYLIB) ---

    // 1. Obtener posición actualizada de las físicas
    int posX, posY;
    pbody->GetPhysicPosition(posX, posY); // Helper que convierte metros a píxeles

    // 2. Obtener rotación
    // Box2D devuelve radianes, Raylib necesita grados.
    float rotationDegrees = pbody->GetRotation() * RAD2DEG;

    // 3. Dibujar
    // Usamos DrawTexturePro para poder rotar la imagen desde su centro
    Rectangle sourceRec = { 0.0f, 0.0f, (float)this->width,(float)this->height };
    Rectangle destRec = { (float)posX, (float)posY, (float)this->width, (float)this->height };

    // El origen de rotación debe ser el centro de TU RECORTE, no de toda la imagen
    Vector2 origin = { (float)this->width / 2, (float)this->height / 2 };

    DrawTexturePro(texture, sourceRec, destRec, origin, rotationDegrees, WHITE);
}
    return UPDATE_CONTINUE;
}



