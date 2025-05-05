#include "image.h"


typedef struct {
	SDL_Texture* texture;
	char* path;
} ImageFile;


static const int LOG_CATEGORY = SDL_LOG_CATEGORY_RENDER;
static const char* IMAGE_SUBFOLDER = "images/"; // subfolder (where the app was run from) for image files 
static char* IMAGES_DIR = NULL;

static SDL_PropertiesID prop_id;
static SDL_Renderer* renderer = NULL;


static void  CleanupPropertyCallback(void* userdata, void* value) {

	ImageFile* imageFile = (ImageFile*)value;
	if (imageFile->texture) {
		SDL_DestroyTexture(imageFile->texture);
	}
	SDL_free(imageFile->path);
}



static bool load_image(const char* dirname, const char* fname) {
	char* path = NULL;
	SDL_asprintf(&path, "%s%s", dirname, fname);
	if (SDL_GetPathInfo(path, NULL)) { //  image file found
		SDL_Surface* surface = IMG_Load(path);
		if (!surface) {
			SDL_Log("Couldn't load image: %s", SDL_GetError());
			return false;
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_DestroySurface(surface);  /* done with this, the texture has a copy of the pixels now. */
		if (!texture) {
			SDL_LogError(LOG_CATEGORY, "IMAGE: Couldn't create texture: %s", SDL_GetError());
			return false;
		}

		ImageFile* imageFile = new ImageFile();
		imageFile->texture = texture;
		imageFile->path = path;

		if (SDL_HasProperty(prop_id, fname)) {
			SDL_LogError(LOG_CATEGORY, "Image '%s' already loaded.", path);
			return false;
		}

		if (!SDL_SetPointerPropertyWithCleanup(prop_id, fname, imageFile, CleanupPropertyCallback, NULL)) {
			SDL_LogError(LOG_CATEGORY, "Image: Error settings property; %s; %s", path, SDL_GetError());
			return false;
		}
		SDL_LogInfo(LOG_CATEGORY, "Image loaded: %s", path);
		return true;
	}
	else {//  image file NOT found
		SDL_LogError(LOG_CATEGORY, "Image '%s' not found.", path);
		return false;
	}
}


static SDL_EnumerationResult  EnumerateDirectoryCallback(void* userdata, const char* dirname, const char* fname) {
	if (!load_image(dirname, fname)) {
		return SDL_ENUM_FAILURE;
	}
	return SDL_ENUM_CONTINUE;
}

static bool image_load_all() {
	return SDL_EnumerateDirectory(IMAGES_DIR, EnumerateDirectoryCallback, NULL);
}


bool image_init(SDL_Renderer* r) {
	renderer = r;
	SDL_asprintf(&IMAGES_DIR, "%s%s", SDL_GetBasePath(), IMAGE_SUBFOLDER);
	prop_id = SDL_CreateProperties();
	image_load_all();
	return true;
}








bool image_load(const char* image) {
	return load_image(IMAGES_DIR, image);
}

//bool image_unload(const char* image) {
//	ImageFile* imageFile = (ImageFile*)SDL_GetPointerProperty(prop_id, image, NULL);
//	if (imageFile) {
//		SDL_DestroyTexture(imageFile->texture);
//		SDL_ClearProperty(prop_id, image);
//		return true;
//	}
//
//	return false;
//}


void image_render(const char* image, float x, float y) {
	ImageFile* imageFile = (ImageFile*)SDL_GetPointerProperty(prop_id, image, NULL);
	if (imageFile) {
		SDL_FRect dst_rect;
		dst_rect.x = x;
		dst_rect.y = y;
		dst_rect.w = (float)imageFile->texture->w;
		dst_rect.h = (float)imageFile->texture->h;
		SDL_RenderTexture(renderer, imageFile->texture, NULL, &dst_rect);
	}
	else {
		SDL_LogError(LOG_CATEGORY, "Rendering failed: image '%s' not loaded.", image);		
	}
}

void image_free() {
	SDL_DestroyProperties(prop_id);
}