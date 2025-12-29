#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include <math.h>

ModuleRender::ModuleRender(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    background = RAYWHITE;

    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    camera.offset = { 0,0 };
    camera.target = { 0,0 };


}

// Destructor
ModuleRender::~ModuleRender()
{}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	bool ret = true;

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRender::PreUpdate()
{
    ClearBackground(background);
    BeginDrawing();
    //movimiento camara2d
    BeginMode2D(camera);
	return UPDATE_CONTINUE;
}

// Update: debug camera
update_status ModuleRender::Update()
{/*
    ClearBackground(background);*/

    // NOTE: This function setups render batching system for
    // maximum performance, all consecutive Draw() calls are
    // not processed until EndDrawing() is called
    /*BeginDrawing();*/

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRender::PostUpdate()
{
    //if (App->game->current_state == START_MENU)
    //{
    //    // Dibujamos la textura del menú cargada en ModuleGame
    //    DrawTexture(App->game->menu_img, 0, 0, WHITE); //

    //    // Texto de inicio con parpadeo
    //    if ((int)(GetTime() * 2) % 2 == 0)
    //    {
    //        //DrawText("PRESIONAR ENTER PARA INICIAR", GetScreenWidth() / 2 - 170, GetScreenHeight() / 2 + 150, 20, LIGHTGRAY);
    //    }
    //}

    EndMode2D();
    //if (App->game != nullptr)
    //{
    //    if (App->game->current_state == START_MENU)
    //    {
    //        // Importante: Para dibujar UI o menús fijos, debemos estar FUERA del Mode2D
    //        EndMode2D();

    //        // Limpiamos el fondo para que no se vea el mapa/coche por debajo
    //        ClearBackground(BLACK);

    //        // Dibujamos la textura del menú
    //        DrawTexture(App->game->menu_img, 0, 0, WHITE);

    //        // Texto de inicio
    //        if ((int)(GetTime() * 2) % 2 == 0)
    //        {
    //            // Usamos GetScreenWidth() de raylib para centrarlo
    //            ::DrawText("PRESIONAR ENTER PARA INICIAR", GetScreenWidth() / 2 - 170, GetScreenHeight() / 2 + 150, 20, LIGHTGRAY);
    //        }

    //        // Re-activamos el modo 2D si hay otros módulos que lo necesitan después, 
    //        // aunque lo ideal es que este sea el final del dibujo.
    //    }
    //    else
    //    {
    //        // Si no estamos en el menú, cerramos el modo 2D normal del juego
    //        EndMode2D();
    //    }
    //}
    //else
    //{
    //    // Si App->game es nulo, al menos cerramos el modo 2D por seguridad
    //    EndMode2D();
    //}
    // Draw everything in our batch!
    DrawFPS(10, 10);

    EndDrawing();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	return true;
}

void ModuleRender::SetBackgroundColor(Color color)
{
	background = color;
}

// Draw to screen
bool ModuleRender::Draw(Texture2D texture, int x, int y, const Rectangle* section, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;

	float scale = 1.0f;
    Vector2 position = { (float)x, (float)y };
    Rectangle rect = { 0.f, 0.f, (float)texture.width, (float)texture.height };

    if (section != NULL) rect = *section;

    position.x = (float)(x-pivot_x) * scale;
    position.y = (float)(y-pivot_y) * scale;

	rect.width *= scale;
	rect.height *= scale;

    DrawTextureRec(texture, rect, position, WHITE);

	return ret;
}

bool ModuleRender::DrawText(const char * text, int x, int y, Font font, int spacing, Color tint) const
{
    bool ret = true;

    Vector2 position = { (float)x, (float)y };

    DrawTextEx(font, text, position, (float)font.baseSize, (float)spacing, tint);

    return ret;
}
