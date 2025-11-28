#pragma once
#pragma once

#include "Module.h"
#include "Globals.h"
#include "raylib.h"
#include <vector>
#include <string>

// Estructura para guardar la info de cada Tileset
struct TileSet
{
    int firstgid;
    std::string name;
    int tileWidth;
    int tileHeight;
    int spacing;
    int margin;
    int texWidth;
    int texHeight;
    int numTilesWidth;
    int numTilesHeight;
    Texture2D texture;
};

// Estructura para guardar las capas (Layers)
struct MapLayer
{
    std::string name;
    int width;
    int height;
    std::vector<int> data; // Array de GIDs (Global Tile IDs)
};

// Estructura principal del Mapa
struct MapData
{
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    Color backgroundColor;
    std::vector<TileSet*> tilesets;
    std::vector<MapLayer*> layers;
};

class ModuleMap : public Module
{
public:

    ModuleMap(Application* app, bool start_enabled = true);
    virtual ~ModuleMap();

    bool Start() override;
    update_status Update() override;
    bool CleanUp() override;

    // Función principal para cargar el TMX
    bool Load(const char* path);

    // Utilidad para convertir coordenadas de mundo a tiles
    void WorldToMap(int x, int y, int& mapX, int& mapY) const;

public:
    MapData mapData;

private:
    Texture2D tileTexture; // Textura principal (si usas un solo atlas)
};