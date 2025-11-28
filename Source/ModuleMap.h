#pragma once

#include "Module.h"
#include "Globals.h"
#include "raylib.h"
#include <list>
#include <vector>
#include <string>

// Forward declaration para no obligar a incluir pugi aquí
namespace pugi { class xml_node; class xml_document; }

struct TileSet
{
    int firstgid;
    std::string name;
    int tileWidth;
    int tileHeight;
    int spacing;
    int margin;
    int tileCount;
    int columns;
    Texture2D texture;

    // Obtener el rectángulo de recorte de la textura para un ID
    Rectangle GetRect(int gid) {
        Rectangle rect = { 0 };
        int relativeIndex = gid - firstgid;
        if (columns == 0) return rect;

        rect.width = (float)tileWidth;
        rect.height = (float)tileHeight;
        rect.x = (float)(margin + (tileWidth + spacing) * (relativeIndex % columns));
        rect.y = (float)(margin + (tileHeight + spacing) * (relativeIndex / columns));
        return rect;
    }
};

struct MapLayer
{
    std::string name;
    int width;
    int height;
    std::vector<int> tiles; // Array de GIDs

    int Get(int i, int j) const {
        if (i < 0 || i >= width || j < 0 || j >= height) return 0;
        return tiles[(j * width) + i];
    }
};

struct MapData
{
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    std::list<TileSet*> tilesets;
    std::list<MapLayer*> layers;
};

class ModuleMap : public Module
{
public:
    ModuleMap(Application* app, bool start_enabled = true);
    virtual ~ModuleMap();

    bool Start() override;
    update_status Update() override;
    bool CleanUp() override;

    // Carga el mapa desde un archivo .tmx
    bool Load(const char* path);

    // Utilidades
    Vector2 MapToWorld(int x, int y) const;
    Vector2 WorldToMap(int x, int y) const;
    TileSet* GetTilesetFromTileId(int gid) const;

public:
    MapData mapData;
    bool mapLoaded;
};