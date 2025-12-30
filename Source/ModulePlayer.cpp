#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleGame.h"
#include "ModuleMap.h"
#include "CarProperties.h"
#include "ModuleRender.h"
#include<algorithm>
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
    x = App->map->playerSpawnPoint.x;
    y = App->map->playerSpawnPoint.y;

    if (x == 0 && y == 0) {
        x = 100; // 默认值
        y = 300;
        LOG("Warning: No PlayerStart found in map, using default.");
    }

    pbody = App->physics->CreateRectangle(x, y, frameWidth, frameHeight, 1, 0xFFFF);

    this->width = frameWidth;
    this->height = frameHeight;

    if (pbody != nullptr)
    {
        pbody->listener = this;
        // 使用 CarStats 中定义的统一数值
        pbody->body->SetLinearDamping(CarStats::LINEAR_DAMPING);
        pbody->body->SetAngularDamping(CarStats::ANGULAR_DAMPING);

        // 获取并修改 Fixture 属性 (影响质量和碰撞反应)
        b2Fixture* fixture = pbody->body->GetFixtureList();
        if (fixture) {
            fixture->SetDensity(CarStats::DENSITY);
            fixture->SetFriction(CarStats::FRICTION);
            fixture->SetRestitution(CarStats::RESTITUTION);
            pbody->body->ResetMassData(); // 修改密度后必须重置
        }
    }

    // 使用 CarStats 中定义的动力数值
    speed = CarStats::ACCELERATION;
    turn_speed = CarStats::STEERING_SPEED;


    currentMaxSpeed = CarStats::MAX_SPEED; // 初始速度
    boostTimer = 0;

    currentBoostCharge = 0.0f;

    // 这里设定“1/4圈”的距离。
    // 注意：Box2D的单位是米。你需要根据你的地图大小调整这个数值。
    // 如果一圈大概是 2000米，那这里设为 500.0f。
    // 你需要运行游戏测试一下，跑完1/4路程时看日志或UI调整这个值。
    maxBoostCharge = 1500.0f;
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
    // 如果当前不是“游戏中”，禁止玩家控制
    if (App->scene_intro->current_state != INGAME) {
        return UPDATE_CONTINUE;
    }
    float currentSpeed = pbody->body->GetLinearVelocity().Length();
    bool showNotReadyWarning = false;

    if (App->game != nullptr && App->game->game_over == true) return UPDATE_CONTINUE;

    if (App->game != nullptr && App->game->game_over == true) {
        if (pbody != nullptr && pbody->body != nullptr) {
            pbody->body->SetLinearVelocity({ 0, 0 });
            pbody->body->SetAngularVelocity(0);
        }
        return UPDATE_CONTINUE;
    }

    if (boostTimer > 0)
    {
        boostTimer--;

        // Calculamos cu醤to debe bajar la barra en cada frame
        // Si el boost dura 120 frames, restamos 1/120 de la barra cada vez
        float depletionSpeed = maxBoostCharge / 20.0f;
        currentBoostCharge -= depletionSpeed;

        // Evitamos que baje de 0
        if (currentBoostCharge < 0) currentBoostCharge = 0.0f;

        if (boostTimer == 0)
        {
            currentMaxSpeed = CarStats::MAX_SPEED;
            LOG("Boost finalizado - Velocidad normal");
        }
    }

    if (pbody != nullptr)
    {
        // 1. 处理加速计时逻辑
        if (boostTimer > 0) {
            boostTimer--;
            if (boostTimer == 0) {
                currentMaxSpeed = CarStats::MAX_SPEED; // 时间到，恢复原始速度
                LOG("Speed normal");
            }
        }
        float currentSpeed = pbody->body->GetLinearVelocity().Length();

        if (boostTimer <= 0 && currentBoostCharge < maxBoostCharge) {
            currentBoostCharge += currentSpeed * 0.5f;

            if (currentBoostCharge>maxBoostCharge) {
                currentBoostCharge = maxBoostCharge;
                LOG("Boost Ready !");
            }
        }

        if (IsKeyPressed(KEY_H)) {
            showHelpMenu = !showHelpMenu;
        }

        // Si el men?est?abierto, "paramos" el juego devolviendo UPDATE_CONTINUE 
        // pero salt醤donos toda la l骻ica de f韘icas y movimiento de abajo.
        if (showHelpMenu) return UPDATE_CONTINUE;

        if (App->game != nullptr && App->game->game_over == true) return UPDATE_CONTINUE;





        // 2. 使用 currentMaxSpeed 而不是固定值
        // 如果 currentMaxSpeed 还没初始化，就在 Start 里设置 currentMaxSpeed = CarStats::MAX_SPEED
        float maxSpeed = currentMaxSpeed;






        b2Body* b = pbody->body;

        // 1. OBTENER DIRECCI?N Y VELOCIDAD
        float currentAngle = b->GetAngle();
        // IMPORTANTE: Esta fórmula debe ser igual en la IA
        b2Vec2 forwardDir = { (float)sin(currentAngle), (float)-cos(currentAngle) };
        b2Vec2 currentVel = b->GetLinearVelocity();
        float forwardSpeed = currentVel.x * forwardDir.x + currentVel.y * forwardDir.y;

        // 2. FRICCI?N LATERAL (SIN DERRAPE)
        // Sincronizado con KillOrthogonalVelocity de la IA
        b2Vec2 lateralCorrection = { forwardDir.x * forwardSpeed, forwardDir.y * forwardSpeed };
        b->SetLinearVelocity(lateralCorrection);

        // 3. GIRO (STEERING)
        float targetRotVelocity = 0.0f;
        float maxTurnSpeed = CarStats::STEERING_SPEED;

        if (abs(forwardSpeed) > 0.5f) // Mínimo de velocidad para girar
        {
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) targetRotVelocity = -maxTurnSpeed;
            else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) targetRotVelocity = maxTurnSpeed;

            if (forwardSpeed < -0.1f) targetRotVelocity *= -1; // Invertir en reversa
        }

        // Sincroniza este valor (0.2f) con la IA si quieres que giren con la misma agilidad
        float turnSmoothing = 0.2f;
        float currentRot = b->GetAngularVelocity();
        float newRot = currentRot + (targetRotVelocity - currentRot) * turnSmoothing;
        b->SetAngularVelocity(newRot);

        // 4. ACELERACI?N
    /*    float maxSpeed = CarStats::MAX_SPEED;*/

        //if (IsKeyPressed(KEY_SPACE) && boostTimer <= 0&&currentBoostCharge>=maxBoostCharge) // IsKeyPressed para que solo se active una vez por pulsaci髇
        //{
        //    boostTimer = 120; // 2 segundos a 60 FPS
        //    currentMaxSpeed = CarStats::MAX_SPEED * 1.5f; // Aumentamos la velocidad m醲ima (ejemplo: x1.5)
        //    currentBoostCharge = 0.0f;
        //    LOG("BOOST ACTIVADO! Change reset");
        //}

        if (IsKeyPressed(KEY_SPACE))
        {
            if (currentBoostCharge >= maxBoostCharge && boostTimer <= 0)
            {
                // ACTIVACI覰 EXITOSA
                boostTimer = 120; // 2 segundos
                currentMaxSpeed = CarStats::MAX_SPEED * 3.5f;
                LOG("BOOST ACTIVADO!");
            }
            else if (boostTimer <= 0)
            {
                // EL JUGADOR PULS?PERO NO EST?LLENO
                showNotReadyWarning = true;
                LOG("Boost no preparado");
            }
        }

        if (IsKeyPressed(KEY_SPACE) && currentBoostCharge < maxBoostCharge && boostTimer <= 0)
        {
            warningTimer = 60; // El mensaje durar?60 frames (1 segundo)
        }

        if (warningTimer > 0) warningTimer--;

        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        {
            if (forwardSpeed < maxSpeed) {
                // 'speed' ya es CarStats::ACCELERATION (asignado en Start)
                b2Vec2 force = { forwardDir.x * speed, forwardDir.y * speed };
                b->ApplyForceToCenter(force, true);
            }
        }

        // 5. FRENADO Y REVERSA
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        {
            if (forwardSpeed > -maxSpeed * 0.5f) {
                b2Vec2 force = { forwardDir.x * -speed * 0.5f, forwardDir.y * -speed * 0.5f };
                b->ApplyForceToCenter(force, true);
            }
        }

        // 6. FRICCI?N NATURAL (Cuando no aceleras)
        if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_SPACE)) {
            b2Vec2 v = b->GetLinearVelocity();
            v.x *= 0.98f; // Sincroniza este 0.98f con la IA
            v.y *= 0.98f;
            b->SetLinearVelocity(v);
        }
        //Get player position
        int px;
        int py;
        pbody->GetPhysicPosition(px,py);
        // --- 镜头跟随与限制逻辑 (Camera Clamping) ---

   // 2. 获取地图的总宽高 (单位：像素)
        // mapData.width 是格子的数量，tileWidth 是一个格子的像素宽
        int mapWidth = App->map->mapData.width * App->map->mapData.tileWidth;
        int mapHeight = App->map->mapData.height * App->map->mapData.tileHeight;

        // 3. 获取屏幕尺寸和当前的缩放倍率
        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();
        float zoom = App->renderer->camera.zoom;

        // 4. 计算“视野的一半”有多大 (在地图世界中的尺寸)
        // 比如屏幕宽800，缩放1.0，那视野半径就是400。如果缩放2.0，视野半径就是200。
        float halfViewW = (screenW / 2.0f) / zoom;
        float halfViewH = (screenH / 2.0f) / zoom;

        // 5. 计算目标位置，但增加限制 (Clamping)
        Vector2 target;
        target.x = (float)px;
        target.y = (float)py;

        // --- X轴限制 ---
        // 只有当地图比屏幕宽时才限制，否则就居中
        if (mapWidth > screenW / zoom) {
            // 如果玩家太靠左，相机停在最左边 (halfViewW)
            if (target.x < halfViewW) target.x = halfViewW;
            // 如果玩家太靠右，相机停在最右边 (地图总宽 - halfViewW)
            else if (target.x > mapWidth - halfViewW) target.x = mapWidth - halfViewW;
        }
        else {
            target.x = mapWidth / 2.0f; // 地图太小，直接居中
        }

        // --- Y轴限制 ---
        if (mapHeight > screenH / zoom) {
            // 如果玩家太靠上
            if (target.y < halfViewH) target.y = halfViewH;
            // 如果玩家太靠下
            else if (target.y > mapHeight - halfViewH) target.y = mapHeight - halfViewH;
        }
        else {
            target.y = mapHeight / 2.0f; // 地图太小，直接居中
        }

        // 6. 应用目标位置
        App->renderer->camera.target = target;

        // 偏移量永远是屏幕中心 (因为我们要把 target 放在屏幕中间显示)
        App->renderer->camera.offset = { screenW / 2.0f, screenH / 2.0f };
    }

    

    return UPDATE_CONTINUE;
}



update_status ModulePlayer::PostUpdate()
{
    //if (App->game == nullptr) return UPDATE_CONTINUE;
    //if (App->game->current_state == START_MENU) {
    //    // (Aqu?va tu c骴igo de dibujo de menu_img que pusimos antes)
    //    return UPDATE_CONTINUE;
    //}
    if (pbody != nullptr)
    {
        // --- RENDERIZADO (RAYLIB) ---

        // 1. Obtener posici髇 actualizada de las f韘icas
        int posX, posY;
        pbody->GetPhysicPosition(posX, posY);

        // 2. Obtener rotaci髇
        float rotationDegrees = pbody->GetRotation() * RAD2DEG;

        // 3. Dibujar
        Rectangle sourceRec = { 0.0f, 0.0f, (float)this->width,(float)this->height };
        Rectangle destRec = { (float)posX, (float)posY, (float)this->width, (float)this->height };
        Vector2 origin = { (float)this->width / 2, (float)this->height / 2 };

        DrawTexturePro(texture, sourceRec, destRec, origin, rotationDegrees, WHITE);

        EndMode2D();
        //UI boost with space
        int barWidth = 200;
        int barHeight = 20;
        int screenW = GetScreenWidth();
        int magin = 80;
        int barX = screenW - barWidth - magin;
        int barY = 30;

        float percentage = currentBoostCharge / maxBoostCharge;

        if (percentage > 1.0f)percentage = 1.0f;

        DrawRectangle(barX, barY, barWidth, barHeight, Fade(DARKGRAY, 0.8f));
        DrawRectangleLines(barX,barY,barWidth,barHeight,WHITE);//margen

        Color barColor = YELLOW;

        if (percentage >= 1.0f) {
            barColor = GREEN;

            DrawText("Boost Ready [SPACE]", barX, barY + 25, 10, GREEN);
        }
        
        DrawRectangle(barX, barY, (int)(barWidth * percentage), barHeight, barColor);

        if (boostTimer > 0) {
            DrawText("BOOSTING!!", barX, barY + 25, 10, RED);

            DrawRectangle(barX,barY,barWidth,barHeight,RED);
        }

        if (warningTimer > 0)
        {
            // Lo dibujamos cerca de la barra o en el centro
            DrawText("BOOST NO PREPARADO", barX - 20, barY + 45, 25, BLUE);
        }

        // Aseg鷕ate de que este bloque se mantenga para ver cuando S?funciona
        if (percentage >= 1.0f && boostTimer <= 0) {
            DrawText("Boost Ready [SPACE]", barX, barY + 25, 25, BLUE);
        }

        if (showHelpMenu) {
            int screenW = GetScreenWidth();
            int screenH = GetScreenHeight();

            // 1. Dibujar un fondo oscuro semitransparente que cubra toda la pantalla
            DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.6f));

            // 2. Dibujar la "ventana" central
            int winW = 400;
            int winH = 300;
            int winX = (screenW - winW) / 2;
            int winY = (screenH - winH) / 2;

            DrawRectangle(winX, winY, winW, winH, RAYWHITE);
            DrawRectangleLines(winX, winY, winW, winH, DARKGRAY);

            // 3. Dibujar el texto de los controles
            DrawText("CONTROLES DEL JUGADOR", winX + 50, winY + 30, 20, MAROON);

            DrawText("- W / Flecha Arriba: Acelerar", winX + 40, winY + 80, 18, DARKGRAY);
            DrawText("- S / Flecha Abajo: Frenar/Reversa", winX + 40, winY + 110, 18, DARKGRAY);
            DrawText("- A-D / Flechas Izq-Der: Girar", winX + 40, winY + 140, 18, DARKGRAY);
            DrawText("- ESPACIO: Activar Boost (si est?lleno)", winX + 40, winY + 170, 18, DARKGRAY);
            DrawText("- H: Cerrar este menu", winX + 40, winY + 210, 18, BLUE);

            DrawText("EL JUEGO ESTA EN PAUSA", winX + 80, winY + 260, 15, RED);
        }

        if (pbody != nullptr)
        {
            // 1. Obtener la velocidad actual (magnitud del vector)
            float speedVal = pbody->body->GetLinearVelocity().Length();

            // 2. Convertir a una cadena de texto (multiplicamos por 10 para que el n鷐ero sea m醩 vistoso)
            static char speedText[32];
            sprintf_s(speedText, 32, "VELOCIDAD: %.1f KM/H", speedVal * 9.0f);

            // 3. Definir posici髇 en pantalla (por ejemplo, esquina inferior derecha)
            int screenW = GetScreenWidth();
            int screenH = GetScreenHeight();
            int posX = screenW - 250;
            int posY = screenH - 50;

            // 4. Dibujar un peque駉 fondo para el texto
            DrawRectangle(posX - 10, posY - 5, 230, 30, Fade(BLACK, 0.5f));

            // 5. Dibujar el texto de velocidad
            // Si la velocidad es alta (por boost), la pintamos en un color diferente
            Color velocityColor = (boostTimer > 0) ? GOLD : WHITE;
            DrawText(speedText, posX, posY, 20, velocityColor);
        }



        BeginMode2D(App->renderer->camera);

        

    }

    return UPDATE_CONTINUE;
}

// ModulePlayer.cpp
//void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
//{
//    // bodyB 是玩家撞到的东西
//    if (bodyB != nullptr && bodyB->ptype == BodyType::BOOST)
//    {
//        boostTimer = 60; // 3秒加速 (假设60fps)
//        currentMaxSpeed = CarStats::MAX_SPEED +3.0f; // 速度翻倍
//        LOG("BOOST ACTIVE!");
//    }
//}

void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
    if (bodyB != nullptr && bodyB->ptype == BodyType::BOOST)
    {
        // Si no quieres que afecte al timer de la barra, 
        // podr韆s aplicar un impulso directo o usar una variable diferente.
        // Si solo quieres velocidad sin que la barra se vea afectada, quita el boostTimer aqu?
        currentMaxSpeed = CarStats::MAX_SPEED + 2.0f;
        // boostTimer = 60; <-- Elimina o comenta esta l韓ea para que no afecte a la barra
        LOG("BOOST SUELO ACTIVO!");
    }
}