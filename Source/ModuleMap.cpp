#include "Globals.h"
#include "Application.h"
#include "ModuleMap.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "pugixml.hpp" 

ModuleMap::ModuleMap(Application* app, bool start_enabled) : Module(app, start_enabled), mapLoaded(false)
{
}

ModuleMap::~ModuleMap()
{
}

bool ModuleMap::Start()
{
    LOG("Loading Map");
    return true;
}

update_status ModuleMap::Update()
{

    if (mapLoaded)
    {
        for (const auto& mapLayer : mapData.layers)
        {
            bool shouldDraw = mapLayer->properties.GetPropertyBool("Draw", true);
            if (shouldDraw) {
               /* if (mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value) {*/

                    for (int x = 0; x < mapData.width; ++x)
                    {
                        for (int y = 0; y < mapData.height; ++y)
                        {
                            int gid = mapLayer->Get(x, y);
                            if (gid != 0)
                            {
                                TileSet* tileset = GetTilesetFromTileId(gid);
                                if (tileset != nullptr)
                                {
                                    Rectangle source = tileset->GetRect(gid);
                                    Vector2 pos = MapToWorld(x, y);
                                    // Dibujar con Raylib
                                    DrawTextureRec(tileset->texture, source, pos, WHITE);
                                }
                            }
                        }
                   /* }*/






                }
            }

            
        }
    }

    return UPDATE_CONTINUE;
}

bool ModuleMap::Load(const char* path)
{
    // Limpiar si ya había un mapa cargado
    if (mapLoaded) CleanUp();

    pugi::xml_document mapFile;
    pugi::xml_parse_result result = mapFile.load_file(path);

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", path, result.description());
        return false;
    }

    pugi::xml_node mapNode = mapFile.child("map");
    mapData.width = mapNode.attribute("width").as_int();
    mapData.height = mapNode.attribute("height").as_int();
    mapData.tileWidth = mapNode.attribute("tilewidth").as_int();
    mapData.tileHeight = mapNode.attribute("tileheight").as_int();

    // 1. Cargar Tilesets
    for (pugi::xml_node tilesetNode = mapNode.child("tileset"); tilesetNode; tilesetNode = tilesetNode.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();
        set->firstgid = tilesetNode.attribute("firstgid").as_int();
        set->name = tilesetNode.attribute("name").as_string();
        set->tileWidth = tilesetNode.attribute("tilewidth").as_int();
        set->tileHeight = tilesetNode.attribute("tileheight").as_int();
        set->spacing = tilesetNode.attribute("spacing").as_int();
        set->margin = tilesetNode.attribute("margin").as_int();
        set->columns = tilesetNode.attribute("columns").as_int();

        // Ruta de la imagen
        std::string imgPath = "Assets/"; // Asume carpeta Assets
        std::string source = tilesetNode.child("image").attribute("source").as_string();

        // Limpiar ruta relativa simple
        size_t slash = source.find_last_of("/\\");
        if (slash != std::string::npos) source = source.substr(slash + 1);

        imgPath += source;
        set->texture = LoadTexture(imgPath.c_str());

        mapData.tilesets.push_back(set);
    }

    // 2. Cargar Layers
    for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode; layerNode = layerNode.next_sibling("layer"))
    {
        MapLayer* layer = new MapLayer();
        layer->name = layerNode.attribute("name").as_string();
        layer->width = layerNode.attribute("width").as_int();
        layer->height = layerNode.attribute("height").as_int();


        pugi::xml_node propertiesNode = layerNode.child("properties");
        for (pugi::xml_node propNode = propertiesNode.child("property"); propNode; propNode = propNode.next_sibling("property"))
        {
            Properties::Property* p = new Properties::Property();
            p->name = propNode.attribute("name").as_string();
            // Asumimos que en Tiled usaste "bool"
            p->value = propNode.attribute("value").as_bool();
            layer->properties.propertiesList.push_back(p);
        }

        for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode; tileNode = tileNode.next_sibling("tile"))
        {
            layer->tiles.push_back(tileNode.attribute("gid").as_int());
        }
        mapData.layers.push_back(layer);

        // 3. Crear Colisiones
        if (layer->name == "Collisions" || layer->name == "Colisiones")
        {
            for (int y = 0; y < layer->height; ++y)
            {
                for (int x = 0; x < layer->width; ++x)
                {
                    int gid = layer->Get(x, y);
                    if (gid != 0)
                    {
                        Vector2 pos = MapToWorld(x, y);
                        // Box2D usa el centro, Tiled la esquina superior izquierda
                        int cx = (int)(pos.x + mapData.tileWidth / 2);
                        int cy = (int)(pos.y + mapData.tileHeight / 2);

                        // Crear rectángulo pasándole TODOS los argumentos para evitar error de "too few arguments"
                        // x, y, w, h, category, mask, group
                        PhysBody* b = App->physics->CreateRectangle(cx, cy, mapData.tileWidth, mapData.tileHeight, 0x0001, 0xFFFF, 0);

                        // IMPORTANTE: Hacerlo estático para que no se caiga
                        b->body->SetFixedRotation(true);
                        b->body->SetType(b2_staticBody);
                    }
                }
            }
        }
    }

    mapLoaded = true;
    LOG("Map loaded successfully");
    return true;
}

bool ModuleMap::CleanUp()
{
    LOG("Unloading map");
    for (auto set : mapData.tilesets) {
        UnloadTexture(set->texture);
        delete set;
    }
    mapData.tilesets.clear();

    for (auto layer : mapData.layers) delete layer;
    mapData.layers.clear();

    mapLoaded = false;
    return true;
}

Vector2 ModuleMap::MapToWorld(int x, int y) const
{
    return { (float)(x * mapData.tileWidth), (float)(y * mapData.tileHeight) };
}

Vector2 ModuleMap::WorldToMap(int x, int y) const
{
    return { (float)(x / mapData.tileWidth), (float)(y / mapData.tileHeight) };
}

TileSet* ModuleMap::GetTilesetFromTileId(int gid) const
{
    TileSet* res = nullptr;
    // Buscar el tileset correcto basado en el firstgid
    for (auto set : mapData.tilesets) {
        if (gid >= set->firstgid) {
            res = set;
        }
    }
    return res;
}