module;
#include <SDL3/SDL_properties.h>
#include <SDL_mixer.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_filesystem.h>
export module soundplayer;


#define LOG_CATEGORY 0
SDL_PropertiesID prop_id = SDL_CreateProperties();


/* List of file extensions for supported sound formats.
 If two files have the same name but different extensions,
 the file with the extension that appears first in this list
 will be loaded by 'Sound::load()'.
 */
const char* extensions[] = {".ogg", ".mp3", ".flac", ".wav"};

typedef struct {
	Mix_Chunk* mix_chunk;
	char* path;
} SoundFile;

namespace Sound {

	export bool init() {
		if (!Mix_OpenAudio(0, NULL)) {			
			return false;
		}

		int numChannels = Mix_AllocateChannels(16);
		SDL_LogInfo(LOG_CATEGORY, "Num channels: %d", numChannels);
		return true;
	}

	export bool load(const char* name) {

		for (size_t i = 0; i < sizeof(extensions)  / sizeof(extensions[0]); i++)
		{
			char* path = NULL;
			SDL_asprintf(&path, "%s%s%s", SDL_GetBasePath(), name, extensions[i] );
			if (SDL_GetPathInfo(path, NULL)) { // file with supported extension found; load sound file and return.
				
				SoundFile* soundFile = new SoundFile;
				soundFile->mix_chunk = Mix_LoadWAV(path);
				soundFile->path = path;
								
				if (soundFile->mix_chunk == NULL) {
					SDL_LogError(LOG_CATEGORY, "Couldn't load %s: %s\n", path, SDL_GetError());
					return false;
				}

				SDL_SetPointerProperty(prop_id, name, soundFile);

				return true;
			}
		}
	
		SDL_LogError(LOG_CATEGORY, "Sound '%s' not found in %s", name, SDL_GetBasePath());
		return false;
	}

	export void play(const char* name) {
		SoundFile* soundFile = (SoundFile *) SDL_GetPointerProperty(prop_id, name, NULL);
		if (soundFile) {
			int channel_playing = Mix_PlayChannel(-1, soundFile->mix_chunk, 0);
			SDL_LogInfo(LOG_CATEGORY, "Playing '%s' on channel %d.", soundFile->path, channel_playing);
		}
		else {
			SDL_LogWarn(LOG_CATEGORY, "Sound '%s' not loaded.", name);
		}
	}

	export void free() {
		Mix_CloseAudio();
		SDL_DestroyProperties(prop_id);
		
		/*if (sound) {
			Mix_FreeChunk(sound);
		}*/
	}

}
