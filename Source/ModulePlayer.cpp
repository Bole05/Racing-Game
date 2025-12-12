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

// Update: draw background
//update_status ModulePlayer::Update()
//{
//    if (App->game != nullptr && App->game->game_over == true)
//    {
//        return UPDATE_CONTINUE;
//    }
//
//    if (pbody != nullptr)
//    {
//
//        
//
//        b2Body* b = pbody->body; // Acceso directo a Box2D
//        float currentAngle = b->GetAngle();
//        b2Vec2 forwardDir = { (float)sin(currentAngle),(float)-cos(currentAngle) };
//        b2Vec2 currentVel = b->GetLinearVelocity();
//
//        float forwardSpeed = currentVel.x * forwardDir.x + currentVel.y * forwardDir.y;
//
//        b2Vec2 lateralCorrection = { forwardDir.x * forwardSpeed, forwardDir.y * forwardSpeed };
//        b->SetLinearVelocity(lateralCorrection);
//
//        float maxSpeed = 10.0f;
//
//        b2Vec2 velocity = b->GetLinearVelocity();
//        float currentSpeed = velocity.Length();
//        float minSpeedToTurn = 0.5f;
//        float turnDrag = 0.96f;
//        // Girar (A/D o Izquierda/Derecha) -> Aplica Torque o velocidad angular
//        if (currentSpeed > minSpeedToTurn) {
//            float targetRotVelocity = 0.0f;
//           /* bool isTurning = false;*/
//            if (IsKeyDown(KEY_LEFT)) {
//                // Opción A: Torque (más realista, con inercia)
//                // b->ApplyTorque(-turn_speed, true);
//                // Opción B: Velocidad directa (más arcade/preciso)
//               /* b->SetAngularVelocity(-turn_speed);*/
//              /*  isTurning = true;*/
//                targetRotVelocity = -turn_speed;
//            }
//            else if (IsKeyDown(KEY_RIGHT)) {
//              /*  b->SetAngularVelocity(turn_speed);
//                isTurning = true;*/
//                targetRotVelocity = turn_speed;
//            }
//            else {
//                // Si no tocas nada, deja de girar (útil para control arcade)
//                b->SetAngularVelocity(0.0f);
//            }
//
//            if (isTurning)
//            {
//                // Obtenemos la velocidad actual y la reducimos
//                b2Vec2 v = b->GetLinearVelocity();
//
//                // Multiplicamos por el factor de frenado (ej: reduce un 4% la velocidad en cada frame)
//                v.x *= turnDrag;
//                v.y *= turnDrag;
//
//                b->SetLinearVelocity(v);
//            }
//        }
//        else {
//            // Si no tocas nada, deja de girar (útil para control arcade)
//            b->SetAngularVelocity(0.0f);
//        }
//      
//
//        // Acelerar (W o Arriba) -> Aplica fuerza en la dirección que mira el coche
//        if (IsKeyDown(KEY_UP))
//        {
//            // Obtener el vector "hacia adelante" basado en el ángulo actual
//            // En Box2D el ángulo 0 suele ser hacia la derecha. Depende de tu sprite.
//            // Asumiendo que el sprite mira hacia arriba o derecha, ajusta el seno/coseno.
//
//            // Si el sprite mira a la DERECHA por defecto:
//            float angle = b->GetAngle();
//            b2Vec2 direction(sin(angle), -cos(angle));
//
//            // Si el sprite mira hacia ARRIBA por defecto, usa:
//            // b2Vec2 direction(sin(angle), -cos(angle)); // Ajustar signos según coordenadas
//
//            // Escalar el vector por la velocidad
//            direction *= speed;
//
//            // Aplicar fuerza en el centro de masa
//            if (currentSpeed < maxSpeed) {
//                b->ApplyForceToCenter(direction, true);
//            }
//          
//        }
//
//        // Frenar / Marcha atrás
//        if (IsKeyDown(KEY_DOWN))
//        {
//            float angle = b->GetAngle();
//            b2Vec2 direction(sin(angle), -cos(angle));
//            direction *= -speed * 0.5f; // Marcha atrás más lenta
//            b->ApplyForceToCenter(direction, true);
//        }
//        if (IsKeyDown(KEY_SPACE))
//        {
//            b2Body* b = pbody->body;
//            b2Vec2 velocity = b->GetLinearVelocity();
//
//            // Factor de frenado:
//            // 0.95f = Frena poco (suave)
//            // 0.90f = Frena normal
//            // 0.80f = Frena fuerte
//            float brakePower = 0.95f;
//
//            velocity.x *= brakePower;
//            velocity.y *= brakePower;
//
//            b->SetLinearVelocity(velocity);
//        }
//        if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
//            b2Vec2 velocity = pbody->body->GetLinearVelocity();
//            velocity.x *= 0.98f;
//            velocity.y *= 0.98f;
//            pbody->body->SetLinearVelocity(velocity);
//    }
//
//    //// --- RENDERIZADO (RAYLIB) ---
//
//    //// 1. Obtener posición actualizada de las físicas
//    //int posX, posY;
//    //pbody->GetPhysicPosition(posX, posY); // Helper que convierte metros a píxeles
//
//    //// 2. Obtener rotación
//    //// Box2D devuelve radianes, Raylib necesita grados.
//    //float rotationDegrees = pbody->GetRotation() * RAD2DEG;
//
//    //// 3. Dibujar
//    //// Usamos DrawTexturePro para poder rotar la imagen desde su centro
//    //Rectangle sourceRec = { 0.0f, 0.0f, (float)this->width,(float)this->height };
//    //Rectangle destRec = { (float)posX, (float)posY, (float)this->width, (float)this->height };
//
//    //// El origen de rotación debe ser el centro de TU RECORTE, no de toda la imagen
//    //Vector2 origin = { (float)this->width / 2, (float)this->height / 2 };
//
//    //DrawTexturePro(texture, sourceRec, destRec, origin, rotationDegrees, WHITE);
//}
//    return UPDATE_CONTINUE;
//}
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

