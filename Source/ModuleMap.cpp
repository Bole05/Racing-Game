#include "Globals.h"
#include "Application.h"
#include "ModuleMap.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "pugixml.hpp" 
#include<string.h>
#include<sstream>
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
    int Coords_Int[196]{
    255, 128,
    288, 130,
    320, 140,
    337, 150,
    350, 160,
    362, 176,
    370, 190,
    376, 208,
    380, 221,
    382, 238,
    383, 255,
    383, 503,
    390, 514,
    400, 528,
    412, 537,
    424, 544,
    434, 549,
    445, 552,
    463, 555,
    479, 556,
    544, 556,
    558, 555,
    573, 553,
    587, 549,
    599, 545,
    612, 537,
    624, 528,
    635, 515,
    640, 506,
    640, 309,
    641, 297,
    642, 288,
    646, 274,
    653, 254,
    664, 236,
    673, 225,
    685, 215,
    701, 205,
    734, 195,
    768, 192,
    800, 192,
    1556, 192,
    1556, 227,
    1553, 236,
    1545, 243,
    1535, 244,
    787, 244,
    765, 249,
    746, 255,
    733, 261,
    721, 272,
    706, 290,
    694, 322,
    692, 350,
    691, 384,
    692, 420,
    693, 464,
    697, 487,
    706, 508,
    720, 526,
    730, 536,
    742, 543,
    760, 550,
    772, 554,
    786, 555,
    799, 555,
    1442, 556,
    1452, 558,
    1458, 563,
    1459, 573,
    1459, 608,
    408, 608,
    256, 608,
    237, 608,
    223, 604,
    205, 600,
    188, 592,
    175, 585,
    161, 575,
    151, 560,
    143, 545,
    135, 527,
    131, 514,
    128, 493,
    127, 450,
    128, 250,
    130, 234,
    132, 222,
    137, 199,
    146, 182,
    153, 171,
    161, 161,
    174, 151,
    190, 142,
    207, 135,
    222, 132,
    237, 129,
    255, 127
    };
    for (int i= 0; i < 196; i++) {
        PIXEL_TO_METERS(Coords_Int[i]);
    }
    App->physics->CreateChain(0,0,Coords_Int,196);

    int Coords_Mid[136]{
    720, 746,
    1604, 747,
    1627, 743,
    1655, 736,
    1680, 719,
    1696, 695,
    1706, 667,
    1708, 639,
    1707, 511,
    1705, 483,
    1695, 456,
    1680, 431,
    1655, 415,
    1626, 405,
    1602, 404,
    831, 404,
    816, 398,
    811, 383,
    817, 368,
    831, 363,
    1760, 364,
    1788, 361,
    1815, 352,
    1840, 336,
    1856, 311,
    1865, 281,
    1868, 256,
    1867, 126,
    1864, 100,
    1855, 72,
    1840, 47,
    1815, 31,
    1788, 23,
    1763, 21,
    640, 20,
    611, 22,
    585, 32,
    560, 48,
    543, 72,
    534, 98,
    531, 127,
    531, 388,
    525, 401,
    510, 403,
    496, 398,
    491, 387,
    491, 128,
    490, 99,
    481, 72,
    462, 48,
    439, 30,
    412, 23,
    383, 19,
    126, 20,
    100, 23,
    72, 32,
    47, 50,
    32, 71,
    22, 99,
    19, 125,
    18, 640,
    22, 668,
    32, 695,
    47, 720,
    72, 736,
    99, 745,
    129, 747,
    703, 747,
    };
    for (int i = 0; i < 136; i++) {
        PIXEL_TO_METERS(Coords_Mid[136]);
    }
    App->physics->CreateChain(0, 0, Coords_Mid, 136);

    int Coords_ext[120]{
            703, 766,
    128, 768,
    92, 764,
    62, 753,
    33, 734,
    12, 705,
    2, 671,
    -1, 638,
    -1, 125,
    2, 94,
    14, 61,
    32, 33,
    64, 13,
    94, 2,
    128, 0,
    384, 0,
    418, 2,
    451, 13,
    479, 33,
    497, 62,
    507, 94,
    512, 128,
    516, 93,
    524, 61,
    546, 34,
    574, 14,
    607, 3,
    641, 0,
    1766, 0,
    1791, 2,
    1826, 13,
    1852, 33,
    1874, 62,
    1884, 93,
    1888, 127,
    1888, 159,
    1888, 192,
    1888, 224,
    1888, 254,
    1884, 289,
    1875, 323,
    1855, 350,
    1826, 369,
    1793, 381,
    1759, 383,
    1601, 384,
    1635, 389,
    1665, 396,
    1693, 417,
    1713, 446,
    1724, 478,
    1727, 510,
    1727, 640,
    1726, 672,
    1713, 706,
    1693, 735,
    1664, 753,
    1633, 763,
    1593, 767,
    737, 767
    };
    for (int i = 0; i < 120; i++) {
        PIXEL_TO_METERS(Coords_ext[120]);
    }
    App->physics->CreateChain(0,0,Coords_ext,120);
    //-------------------------------------------------------------------------------------------------------------------------------------------------//
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
    // Limpiar si ya hab韆 un mapa cargado
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

                        // Crear rect醤gulo pas醤dole TODOS los argumentos para evitar error de "too few arguments"
                        // x, y, w, h, category, mask, group
                        PhysBody* b = App->physics->CreateRectangle(cx, cy, mapData.tileWidth, mapData.tileHeight, 0x0001, 0xFFFF, 0);

                        // IMPORTANTE: Hacerlo est醫ico para que no se caiga
                        b->body->SetFixedRotation(true);
                        b->body->SetType(b2_staticBody);
                    }
                }
            }
        }
    }
    // 4. Cargar Ruta de IA (Object Layer llamada "Path")
     trackPaths.clear();
    pugi::xml_node objectGroup = mapNode.child("objectgroup");
    for (; objectGroup; objectGroup = objectGroup.next_sibling("objectgroup"))
    {
        std::string name = objectGroup.attribute("name").as_string();
        if (name == "Path"|| name=="Paths") // Buscamos la capa espec韋ica
        {
           /* trackPaths.clear();*/

            for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object")) {
                std::vector<b2Vec2> newPath;

                int originX = object.attribute("x").as_int();
                int originY = object.attribute("y").as_int();


                pugi::xml_node polyline = object.child("polyline");
              
                std::string pointsString = polyline.attribute("points").as_string();

                // Parsear el string de puntos
                std::stringstream ss(pointsString);
                std::string pointData;


                while (std::getline(ss, pointData, ' ')) // Separar por espacio
                {
                    size_t commaPos = pointData.find(',');
                    if (commaPos != std::string::npos)
                    {
                        std::string xStr = pointData.substr(0, commaPos);
                        std::string yStr = pointData.substr(commaPos + 1);

                        float x = std::stof(xStr) + originX;
                        float y = std::stof(yStr) + originY;

                        // Convertir de P韝eles a Metros y guardar
                        newPath.push_back({ PIXEL_TO_METERS(x), PIXEL_TO_METERS(y) });
                    }
                }
                /*LOG("AI Path loaded with %d points", trackPath.size());*/
                if (!newPath.empty()) {
                    trackPaths.push_back(newPath);
                    LOG("Loaded path with %d points", newPath.size());
                }

            }

        }

        if (name=="Powerups") {
            for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object")) {
                std::string type = object.attribute("type").as_string(); // Tiled 的 "Class" 或 "Type" 字段

                // 或者如果使用的是自定义属性，请用这段:
                /* pugi::xml_node props = object.child("properties");
                for (pugi::xml_node p = props.child("property"); p; p = p.next_sibling("property")) {
                    std::string propName = p.attribute("name").as_string();
                    if (propName == "Type") type = p.attribute("value").as_string();
                }
                */

                // 如果检测到是加速带
                if (type == "Boost")
                {
                    int x = object.attribute("x").as_int();
                    int y = object.attribute("y").as_int();
                    int width = object.attribute("width").as_int();
                    int height = object.attribute("height").as_int();

                    // Tiled 的坐标是左上角，Box2D 通常以中心点为原点创建矩形
                    // 我们需要把坐标偏移到中心
                    int centerX = x + width / 2;
                    int centerY = y + height / 2;

                    // 创建传感器 (Sensor)
                    PhysBody* boost = App->physics->CreateRectangleSensor(centerX, centerY, width, height);
                    boost->ptype = BodyType::BOOST; // 设置类型

                    // 也可以设置 listener 为 ModulePlayer 或 ModuleGame，以便接收回调
                    // boost->listener = App->player; 

                    LOG("BOOST");
                }


            }










        }


















    }
    // -------------------------------------------------------------------------
        // 5. Cargar Spawn Points (Object Layer llamada "Spawn")
        // -------------------------------------------------------------------------

        // 重新从 map 节点获取第一个 objectgroup
    for (pugi::xml_node spawnLayer = mapNode.child("objectgroup"); spawnLayer; spawnLayer = spawnLayer.next_sibling("objectgroup"))
    {
        std::string name = spawnLayer.attribute("name").as_string();

        // 只有当这一层的名字叫 "Spawn" 时，才读取里面的对象
        // (请确保你在 Tiled 软件里把层命名为 "Spawn"，区分大小写)
        if (name == "Spawn")
        {
            for (pugi::xml_node object = spawnLayer.child("object"); object; object = object.next_sibling("object"))
            {
                std::string objectName = object.attribute("name").as_string();
               
                float x = object.attribute("x").as_float();
                float y = object.attribute("y").as_float();

                if (objectName == "PlayerStart") // 只有名字完全匹配才设为玩家
                {
                    playerSpawnPoint = b2Vec2(x, y); // 存储像素坐标
                    LOG("Player Spawn found at: %f, %f", x, y);
                }
                else if (objectName == "EnemyStart") // 只有名字匹配才加入敌人列表
                {
                    enemySpawnPoints.push_back(b2Vec2(x, y));
                    LOG("Enemy Spawn found at: %f, %f", x, y);
                }


      
            }
        }
    }

    LOG("TOTAL PATHS LOADED: %d", trackPaths.size());
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