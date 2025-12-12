#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleGame.h"
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
    x = 100;
    y = 300;
    pbody = App->physics->CreateRectangle(x, y, frameWidth, frameHeight, 1, 0xFFFF);

    this->width = frameWidth;
    this->height = frameHeight;
	if (pbody != nullptr)
	{
        pbody->listener = this;
		// Damping: "Freno" natural. Si sueltas el gas, el coche para.
		pbody->body->SetLinearDamping(0.2f);  // Fricción de movimiento
		pbody->body->SetAngularDamping(2.0f); // Fricción de rotación
	}

	// Ajustar variables de velocidad
	speed = 7.0f;       // Fuerza de aceleración
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


update_status ModulePlayer::Update()
{
    if (App->game != nullptr && App->game->game_over == true)
    {
        return UPDATE_CONTINUE;
    }

    if (pbody != nullptr)
    {
        b2Body* b = pbody->body;

        // ----------------------------------------------------------------
        // 1. FRICCIÓN LATERAL (NEUMÁTICOS)
        // ----------------------------------------------------------------
        // Elimina la velocidad lateral para que el coche vaya hacia donde mira
        // y no "derrape" como si estuviera en hielo.

        float currentAngle = b->GetAngle();
        // Calculamos el vector "hacia adelante" según el ángulo del coche
        b2Vec2 forwardDir = { (float)sin(currentAngle), (float)-cos(currentAngle) };

        // Velocidad actual del cuerpo
        b2Vec2 currentVel = b->GetLinearVelocity();

        // Proyectamos la velocidad sobre la dirección frontal (dot product)
        float forwardSpeed = currentVel.x * forwardDir.x + currentVel.y * forwardDir.y;

        // Recalculamos la velocidad conservando SOLO la componente frontal
        // (El 0.95f permite un 5% de derrape para que no sea totalmente rígido)
        b2Vec2 lateralCorrection = { forwardDir.x * forwardSpeed, forwardDir.y * forwardSpeed };
        b->SetLinearVelocity(lateralCorrection);

        // ----------------------------------------------------------------
        // 2. GIRO SUAVIZADO (STEERING)
        // ----------------------------------------------------------------

        float targetRotVelocity = 0.0f;

        // Define aquí la velocidad máxima de giro (asegúrate de que sea alta, ej: 6.0f)
        // Si usas la variable de clase 'turn_speed', asegúrate de haberla puesto a 6.0f en Start()
        float maxTurnSpeed = 6.0f;

        // Solo permitimos girar si el coche se está moviendo un mínimo
        if (abs(forwardSpeed) > 0.5f)
        {
            if (IsKeyDown(KEY_LEFT)) {
                targetRotVelocity = -maxTurnSpeed;
            }
            else if (IsKeyDown(KEY_RIGHT)) {
                targetRotVelocity = maxTurnSpeed;
            }

            // Invertir giro si vamos marcha atrás para control natural
            if (forwardSpeed < -0.1f) targetRotVelocity *= -1;
        }

        // Interpolación (Suavizado):
        // 0.1f = Muy suave (lento)
        // 0.3f = Equilibrado
        // 1.0f = Instantáneo (brusco)
        float turnSmoothing = 0.2f;

        float currentRot = b->GetAngularVelocity();
        float newRot = currentRot + (targetRotVelocity - currentRot) * turnSmoothing;
        b->SetAngularVelocity(newRot);

        // ----------------------------------------------------------------
        // 3. ACELERACIÓN Y FRENADO
        // ----------------------------------------------------------------

        float maxSpeed = 10.0f;
        // Vector dirección actualizado
        b2Vec2 direction = forwardDir;

        // Acelerar
        if (IsKeyDown(KEY_UP))
        {
            if (forwardSpeed < maxSpeed) {
                b2Vec2 force = { direction.x * speed, direction.y * speed };
                b->ApplyForceToCenter(force, true);
            }
        }

        // Marcha atrás / Frenar
        if (IsKeyDown(KEY_DOWN))
        {
            if (forwardSpeed > -maxSpeed * 0.5f) { // Límite marcha atrás
                b2Vec2 force = { direction.x * -speed * 0.5f, direction.y * -speed * 0.5f };
                b->ApplyForceToCenter(force, true);
            }
        }

        // Freno de mano (Espacio)
        if (IsKeyDown(KEY_SPACE))
        {
            // Frena un 5% cada frame
            b2Vec2 v = b->GetLinearVelocity();
            v.x *= 0.95f;
            v.y *= 0.95f;
            b->SetLinearVelocity(v);
        }

        // Fricción natural (si no aceleras, el coche se para solo poco a poco)
        if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
            b2Vec2 v = b->GetLinearVelocity();
            v.x *= 0.98f;
            v.y *= 0.98f;
            b->SetLinearVelocity(v);
        }
    }

    // Nota: El renderizado se mueve a PostUpdate en muchos engines, 
    // pero si tu estructura lo requiere aquí, déjalo aquí.
    // He omitido el bloque de renderizado comentado para mantenerlo limpio,
    // ya que tu código original tenía uno en PostUpdate también.

    return UPDATE_CONTINUE;
}

update_status ModulePlayer::PostUpdate()
{
    if (pbody != nullptr)
    {
        // --- RENDERIZADO (RAYLIB) ---

        // 1. Obtener posición actualizada de las físicas
        int posX, posY;
        pbody->GetPhysicPosition(posX, posY);

        // 2. Obtener rotación
        float rotationDegrees = pbody->GetRotation() * RAD2DEG;

        // 3. Dibujar
        Rectangle sourceRec = { 0.0f, 0.0f, (float)this->width,(float)this->height };
        Rectangle destRec = { (float)posX, (float)posY, (float)this->width, (float)this->height };
        Vector2 origin = { (float)this->width / 2, (float)this->height / 2 };

        DrawTexturePro(texture, sourceRec, destRec, origin, rotationDegrees, WHITE);
    }

    return UPDATE_CONTINUE;
}

