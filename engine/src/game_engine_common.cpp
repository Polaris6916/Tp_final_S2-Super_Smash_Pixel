/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game_engine_common.h"
//#include "utils/asset_manager.h"

#ifndef AssertNew
#define AssertNew(ptr) { if (ptr == NULL) { assert(false); abort(); } }
#endif

Timer *g_time = nullptr;
SDL_Renderer *g_renderer = NULL;
SDL_Window *g_window = NULL;
TTF_TextEngine *g_textEngine = NULL;

static int g_rendererW = 0;
static int g_rendererH = 0;

SDL_AppResult game::Init(int sdlFlags, int mixFlags)
{
    // Initialise la SDL3
    if (!SDL_Init(sdlFlags))
    {
        std::cout << "ERROR - SDL_Init " << SDL_GetError() << std::endl;
        assert(false);
        return SDL_APP_FAILURE;
    }

    // Initialise la SDL3 TTF
    if (!TTF_Init())
    {
        std::cout << "ERROR - TTF_Init " << SDL_GetError() << std::endl;
        assert(false);
        return SDL_APP_FAILURE;
    }

    // Initialise la SDL2 Mixer
    if (Mix_Init(mixFlags) != mixFlags)
    {
        std::cout << "ERROR - Mix_Init " << SDL_GetError() << std::endl;
        assert(false);
        abort();
    }
    SDL_AudioDeviceID audioDeviceID = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;
    SDL_AudioSpec audioSpec = { SDL_AUDIO_UNKNOWN, 0, 0 };
    int sampleFrames = 0;
    bool success = SDL_GetAudioDeviceFormat(audioDeviceID, &audioSpec, &sampleFrames);
    std::cout
        << "INFO - Audio format:" << std::endl
        << "     - Channel count = " << audioSpec.channels << std::endl
        << "     - Frequency = " << audioSpec.freq << std::endl
        << "     - Format = ";

    switch (audioSpec.format)
    {
    case SDL_AUDIO_S16: std::cout << "SDL_AUDIO_S16" << std::endl; break;
    case SDL_AUDIO_S32: std::cout << "SDL_AUDIO_S32" << std::endl; break;
    case SDL_AUDIO_F32: std::cout << "SDL_AUDIO_F32" << std::endl; break;
    default:            std::cout << "UNKNOWN" << std::endl; break;
        break;
    }

    if (!Mix_OpenAudio(audioDeviceID, &audioSpec))
    {
        std::cout << "ERROR - Mix_OpenAudio " << SDL_GetError() << std::endl;
        assert(false);
        abort();
    }
    int count = Mix_AllocateChannels(MIX_CHANNEL_COUNT);
    assert(count == MIX_CHANNEL_COUNT);

    // Crée le temps global du jeu
    g_time = new Timer();

    // Initialise Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    return SDL_APP_CONTINUE;
}

SDL_AppResult game::CreateWindow(int width, int height, const std::string &title, Uint32 flags)
{
    assert(g_window == NULL && "The window is already created");

    g_window = SDL_CreateWindow(title.c_str(), width, height, flags);
    if (!g_window)
    {
        std::cout << "ERROR - Create window " << SDL_GetError() << std::endl;
        assert(false);
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult game::CreateRenderer(int width, int height)
{
    assert(g_renderer == NULL && "The renderer is already created");
    assert(g_window);
    bool success = true;

    std::cout << "INFO - Available rendering engines:" << std::endl;
    for (int i = 0; i < SDL_GetNumRenderDrivers(); i++)
    {
        std::cout << "     - " << SDL_GetRenderDriver(i) << std::endl;
    }

    //g_renderer = SDL_CreateRenderer(g_window, "direct3d11");
    //g_renderer = SDL_CreateRenderer(g_window, "direct3d12");
    //g_renderer = SDL_CreateRenderer(g_window, "opengl");
    //g_renderer = SDL_CreateRenderer(g_window, "opengl");
    g_renderer = SDL_CreateRenderer(g_window, NULL);

    std::cout << "INFO - Selected renderer:" << SDL_GetRendererName(g_renderer) << std::endl;

    if (!g_renderer)
    {
        std::cout << "ERROR - Create renderer " << SDL_GetError() << std::endl;
        assert(false);
        return SDL_APP_FAILURE;
    }
    g_rendererW = width;
    g_rendererH = height;

    success = SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);
    assert(success);

    if (!SDL_SetRenderVSync(g_renderer, 1))
    {
        std::cout << "ERROR - Render VSync " << SDL_GetError() << std::endl;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(g_window, g_renderer);
    ImGui_ImplSDLRenderer3_Init(g_renderer);

    g_textEngine = TTF_CreateRendererTextEngine(g_renderer);
    if (!g_textEngine)
    {
        std::cout << "ERROR - Create text engine " << SDL_GetError() << std::endl;
        assert(false);
        return SDL_APP_FAILURE;
    }


    return SDL_APP_CONTINUE;
}

void game::DestroyRenderer()
{
    if (!g_renderer) return;
    assert(g_textEngine);

    TTF_DestroyRendererTextEngine(g_textEngine);
    g_textEngine = NULL;
    SDL_DestroyRenderer(g_renderer);
    g_renderer = NULL;
}

void game::DestroyWindow()
{
    if (!g_window) return;
    SDL_DestroyWindow(g_window);
    g_window = NULL;
}

void game::Quit()
{
    assert(g_window == NULL);
    assert(g_renderer == NULL);
    assert(g_textEngine == NULL);

    delete g_time; g_time = nullptr;

    //Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}

void game::SetMusicVolume(float volume)
{
    int v = (int)(fminf(fmaxf(volume, 0.f), 1.f) * MIX_MAX_VOLUME);
    Mix_VolumeMusic(v);
}

void Game_setSoundVolume(AssetManager *assets, int soundID, float volume)
{
    assert(assets && "The asset manager must be created");
    //Mix_Chunk *chunk = AssetManager_getSound(assets, soundID);
    //if (chunk == NULL)
    //{
    //    assert(false);
    //    return;
    //}
    //int v = (int)(fminf(fmaxf(volume, 0.f), 1.f) * MIX_MAX_VOLUME);
    //Mix_VolumeChunk(chunk, v);
}

void game::SetFXChannelsVolume(float volume)
{
    //int v = (int)(fminf(fmaxf(volume, 0.f), 1.f) * MIX_MAX_VOLUME);
    //for (int channel = 0; channel < MIX_CHANNEL_COUNT; channel++)
    //{
    //    Mix_Volume(channel, v);
    //}
}

void Game_playSoundFX(AssetManager *assets, int soundID)
{
    //assert(assets && "The asset manager must be created");
    //Mix_Chunk *chunk = AssetManager_getSound(assets, soundID);
    //if (chunk == NULL)
    //{
    //    assert(false);
    //    return;
    //}
    //int loops = 0;
    //int exitStatus = Mix_PlayChannel(-1, chunk, loops);
}

void Game_playMusic(AssetManager *assets, int musicID)
{
    //assert(assets && "The asset manager must be created");
    //Mix_Music *music = AssetManager_getMusic(assets, musicID);
    //if (music == NULL)
    //{
    //    assert(false);
    //    return;
    //}
    //int loops = 0;
    //int exitStatus = Mix_PlayMusic(music, loops);
}

void Game_setRenderDrawColor(SDL_Color color, Uint8 alpha)
{
    assert(g_renderer && "The renderer must be created");
    SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, alpha);
}

void Memcpy(void *const dst, size_t dstSize, const void *src, size_t srcSize)
{
#ifdef _MSC_VER
    memcpy_s(dst, dstSize, src, srcSize);
#else
    memcpy(dst, src, (srcSize < dstSize) ? srcSize : dstSize);
#endif
}

char *Strdup(const char *src)
{
    size_t size = strlen(src);
    char *dst = (char *)calloc(size + 1, sizeof(char));
    AssertNew(dst);
    Memcpy(dst, size, src, size);
    return dst;
}

int Game_getWidth()
{
    return g_rendererW;
}

int Game_getHeight()
{
    return g_rendererH;
}

float game::GetViewportScale()
{
    SDL_Rect viewport = { 0 };
    SDL_GetRenderViewport(g_renderer, &viewport);
    return static_cast<float>(viewport.w) / 1920.0f;
}
