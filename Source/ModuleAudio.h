#pragma once

#include "Module.h"

#define MAX_SOUNDS	16
#define DEFAULT_MUSIC_FADE_TIME 2.0f

class ModuleAudio : public Module
{
public:

	ModuleAudio(Application* app, bool start_enabled = true);
	~ModuleAudio();

	bool Init() override;
	bool CleanUp() override;

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a sound in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded sound
	bool PlayFx(unsigned int fx, int repeat = 0);

	// 新增：专门用于预加载音乐
	void PreloadMusic(const char* path);
	// 新增：专门用于播放已经加载好的音乐
	void PlayPreloadedMusic();

	update_status Update() override;

private:

	Music music;
	Sound fx[MAX_SOUNDS];
    unsigned int fx_count;
	Music preloadedMusic; // 用来存预加载的音乐
	bool hasPreloadedMusic = false;
};
