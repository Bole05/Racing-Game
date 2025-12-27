#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"

#include "raylib.h"

#define MAX_FX_SOUNDS   64

ModuleAudio::ModuleAudio(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	fx_count = 0;
	music = Music{ 0 };
}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init()
{
	LOG("Loading Audio Mixer");
	bool ret = true;

    LOG("Loading raylib audio system");

    InitAudioDevice();

	return ret;
}

update_status ModuleAudio::Update()
{
    //// Esto hace que la música siga sonando
    //if (IsMusicReady(music))
    //{
    //    UpdateMusicStream(music);
    //}
    //return update_status::UPDATE_CONTINUE;

	if (music.stream.buffer != NULL && IsMusicStreamPlaying(music))
	{
		UpdateMusicStream(music);
	}
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

    // Unload sounds
	for (unsigned int i = 0; i < fx_count; i++)
	{
		UnloadSound(fx[i]);
	}

    // Unload music
	if (IsMusicReady(music))
	{
		StopMusicStream(music);
		UnloadMusicStream(music);
	}

    CloseAudioDevice();

	return true;
}

// Play a music file
bool ModuleAudio::PlayMusic(const char* path, float fade_time)
{
	//if(IsEnabled() == false)
	//	return false;

	//bool ret = true;
	//
 //   StopMusicStream(music);
 //   music = LoadMusicStream(path);
 //   
 //   PlayMusicStream(music);

	//LOG("Successfully playing %s", path);

	//return ret;
	if (IsEnabled() == false) return false;

	StopMusicStream(music);
	music = LoadMusicStream(path);

	// Verificación de seguridad
	if (music.stream.buffer == NULL) {
		LOG("ERROR: No se pudo cargar la musica en %s", path);
		return false;
	}

	PlayMusicStream(music);
	SetMusicVolume(music, 0.5f); // Prueba con volumen al 50%
	LOG("Successfully playing %s", path);
	return true;
}

// Load WAV
unsigned int ModuleAudio::LoadFx(const char* path)
{
	if(IsEnabled() == false)
		return 0;

	unsigned int ret = 0;

	Sound sound = LoadSound(path);

	if(sound.stream.buffer == NULL)
	{
		LOG("Cannot load sound: %s", path);
	}
	else
	{
        fx[fx_count] = sound;
		ret = fx_count++;
	}

	return ret;
}

// Play WAV
bool ModuleAudio::PlayFx(unsigned int id, int repeat)
{
	if (IsEnabled() == false)
	{
		return false;
	}

	bool ret = false;

	if(id < fx_count) PlaySound(fx[id]);

	return ret;
}