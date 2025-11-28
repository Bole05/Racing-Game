#include "Globals.h"
#include "Application.h"
#include "ModuleMap.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"

// Si decides usar pugixml, inclúyelo aquí:
// #include "pugixml.hpp" 

ModuleMap::ModuleMap(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleMap::~ModuleMap()
{
}

bool ModuleMap::Start()
{
    LOG("Loading Map");
    bool ret = true;

    // Cargar tu mapa aquí. Ejemplo:
    // Load("Assets/Maps/level1.tmx");

    // Si no tienes parser XML aún, puedes cargar la textura directamente para probar:
    // mapData.tilesets.push_back(new TileSet());
    // mapData.tilesets[0]->texture = LoadTexture("Assets/Maps/tileset.png");

    return ret;
}

bool ModuleMap::Load(const char* path)
{
    // AQUI VA LA LÓGICA DE CARGA XML (Recomendado: pugixml)
    // 1. Cargar archivo XML
    // 2. Leer nodo "map" para obtener width, height, tilewidth, tileheight
    // 3. Iterar nodos "tileset" para cargar las texturas (LoadTexture)
    // 4. Iterar nodos "layer" para llenar el vector de datos (GIDs)

    LOG("Map loaded successfully: %s", path);
    return true;
}

update_status ModuleMap::Update()
{
    // Dibujar el mapa
    for (const auto& layer : mapData.layers)
    {
        // Iterar por cada tile de la capa
        for (int y = 0; y < mapData.height; ++y)
        {
            for (int x = 0; x < mapData.width; ++x)
            {
                // Obtener el ID del tile en esta posición (formula para array 1D)
                int gid = layer->data[y * mapData.width + x];

                if (gid > 0)
                {
                    // Buscar a qué tileset pertenece este GID
                    TileSet* tileset = mapData.tilesets[0]; // Asumiendo 1 tileset por simplicidad

                    // Calcular el rectángulo en la textura (Source Rect)
                    int localId = gid - tileset->firstgid;
                    int tileX = localId % tileset->numTilesWidth;
                    int tileY = localId / tileset->numTilesWidth;

                    Rectangle sourceRec = {
                        (float)(tileset->margin + (tileset->tileWidth + tileset->spacing) * tileX),
                        (float)(tileset->margin + (tileset->tileHeight + tileset->spacing) * tileY),
                        (float)tileset->tileWidth,
                        (float)tileset->tileHeight
                    };

                    // Calcular posición en pantalla
                    Vector2 position = { (float)(x * tileset->tileWidth), (float)(y * tileset->tileHeight) };

                    // Dibujar
                    DrawTextureRec(tileset->texture, sourceRec, position, WHITE);
                }
            }
        }
    }

    return UPDATE_CONTINUE;
}

bool ModuleMap::CleanUp()
{
    LOG("Unloading map");

    // Limpiar tilesets y descargar texturas
    for (auto tileset : mapData.tilesets) {
        UnloadTexture(tileset->texture);
        delete tileset;
    }
    mapData.tilesets.clear();

    // Limpiar capas
    for (auto layer : mapData.layers) {
        delete layer;
    }
    mapData.layers.clear();

    return true;
}