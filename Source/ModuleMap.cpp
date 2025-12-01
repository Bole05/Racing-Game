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
    //--------------------------------------------Collision Map------------------------------------------//
    int Coords_Int[138]{
    674, 608,
    256, 607,
    223, 605,
    190, 594,
    162, 574,
    141, 545,
    131, 514,
    127, 480,
    127, 256,
    131, 220,
    142, 190,
    160, 162,
    190, 142,
    223, 132,
    256, 128,
    291, 131,
    323, 143,
    349, 162,
    370, 190,
    380, 221,
    382, 256,
    383, 446,
    374, 476,
    383, 503,
    400, 527,
    423, 543,
    452, 553,
    480, 556,
    543, 556,
    573, 553,
    599, 544,
    623, 528,
    640, 505,
    649, 476,
    641, 447,
    641, 320,
    642, 286,
    654, 253,
    673, 225,
    701, 205,
    733, 196,
    768, 191,
    1534, 192,
    1551, 207,
    1556, 222,
    1551, 240,
    1535, 244,
    794, 243,
    771, 247,
    745, 254,
    720, 271,
    703, 295,
    693, 322,
    690, 350,
    690, 383,
    691, 416,
    693, 473,
    704, 503,
    720, 528,
    743, 544,
    775, 554,
    799, 556,
    1436, 556,
    1456, 559,
    1458, 576,
    1455, 590,
    1440, 606,
    1406, 608,
    703, 609
    };
    for (int i= 0; i < 128; i++) {
        PIXEL_TO_METERS(Coords_Int[i]);
    }
    App->physics->CreateChain(0,0,Coords_Int,138);





    return true;
}

update_status ModuleMap::Update()
{
    if (mapLoaded)
    {
        for (const auto& mapLayer : mapData.layers)
        {
            // BUSCAMOS LA PROPIEDAD. Si existe la respetamos, si no existe asumimos TRUE (dibujar)
            bool shouldDraw = true;
            Properties::Property* prop = mapLayer->properties.GetProperty("Draw");
            if (prop != nullptr) {
                shouldDraw = prop->value;
            }

            if (shouldDraw)
            {
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
                                DrawTextureRec(tileset->texture, source, pos, WHITE);
                            }
                        }
                    }
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
        //std::string imgPath = "Assets/"; // Asume carpeta Assets
        std::string imgPath = "Assets-racing/Maps/"; // Asume carpeta Assets
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