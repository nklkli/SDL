// Initializes SDL mixer; loads sound files from the file system; plays sounds.
// Use Sound:: namespace to call the functions e.g. Sound::play("hello")
#include <SDL3/SDL_properties.h>
#include <SDL_mixer.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_filesystem.h>



const int LOG_CATEGORY = 0;
const int  NUM_CHANNELS = 16;
static const char* SOUNDS_SUBFOLDER = "sounds/"; // subfolder (where the app was run from) for sound files 
static char* SOUNDS_DIR = NULL;


typedef struct {
	Mix_Chunk* mix_chunk;
	char* path;
} SoundFile;


static SDL_PropertiesID prop_id = NULL;


static void  CleanupPropertyCallback(void* userdata, void* value) {
	SoundFile* soundFile = (SoundFile*)value;
	if (soundFile->mix_chunk) {
		Mix_FreeChunk(soundFile->mix_chunk);
	}
	SDL_free(soundFile->path);
}

static bool load_sound(const char* dirname, const char* fname) {
	char* path = NULL;
	SDL_asprintf(&path, "%s%s", dirname, fname);
	if (SDL_GetPathInfo(path, NULL)) { // file with supported extension found; load sound file and return.

		SoundFile* soundFile = new SoundFile;
		soundFile->mix_chunk = Mix_LoadWAV(path);
		soundFile->path = path;

		if (soundFile->mix_chunk == NULL) {
			SDL_LogError(LOG_CATEGORY, "Couldn't load sound '%s'; %s", path, SDL_GetError());
			return false;
		}

		if (SDL_HasProperty(prop_id, fname)) {
			SDL_LogError(LOG_CATEGORY, "Sound '%s' already loaded.", path);
			return false;
		}

		if (!SDL_SetPointerPropertyWithCleanup(prop_id, fname, soundFile, CleanupPropertyCallback, NULL)) {
			SDL_LogError(LOG_CATEGORY, "SOUND: Error settings property '%s'; %s; %s", fname, path, SDL_GetError());
			return false;
		}

		SDL_LogInfo(LOG_CATEGORY, "Sound loaded: %s", path);

		return true;
	}
	else {
		SDL_LogError(LOG_CATEGORY, "Sound '%s' not foun.", path);
		return false;
	}


}

static SDL_EnumerationResult  EnumerateDirectoryCallback(void* userdata, const char* dirname, const char* fname) {
	if (!load_sound(dirname, fname)) {
		return SDL_ENUM_FAILURE;
	}
	return SDL_ENUM_CONTINUE;
}

static bool sound_load_all() {
	return SDL_EnumerateDirectory(SOUNDS_DIR, EnumerateDirectoryCallback, NULL);
}



bool sound_init() {

	SDL_asprintf(&SOUNDS_DIR, "%s%s", SDL_GetBasePath(), SOUNDS_SUBFOLDER);

	prop_id = SDL_CreateProperties();


	if (!Mix_OpenAudio(0, NULL)) {
		return false;
	}

	int numChannels = Mix_AllocateChannels(NUM_CHANNELS);
	SDL_LogInfo(LOG_CATEGORY, "Num audio channels: %d", numChannels);

	sound_load_all();

	return true;
}




bool sound_load(const char* soundfile) {
	return load_sound(SOUNDS_DIR, soundfile);
}

void sound_play(const char* name) {
	SoundFile* soundFile = (SoundFile*)SDL_GetPointerProperty(prop_id, name, NULL);
	if (soundFile) {
		int channel_playing = Mix_PlayChannel(-1, soundFile->mix_chunk, 0);
		SDL_LogInfo(LOG_CATEGORY, "Playing '%s' on channel %d.", soundFile->path, channel_playing);
	}
	else {
		SDL_LogWarn(LOG_CATEGORY, "Sound '%s' not loaded.", name);
	}
}

void sound_free() {
	Mix_CloseAudio();
	SDL_DestroyProperties(prop_id);
}

