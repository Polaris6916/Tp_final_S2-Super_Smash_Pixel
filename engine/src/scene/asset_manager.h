/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "game_engine_common.h"
#include "rendering/sprite_sheet.h"
#include "utils/color.h"

class AssetManager
{
public:
    AssetManager();
    AssetManager(AssetManager const&) = delete;
    AssetManager& operator=(AssetManager const&) = delete;
    ~AssetManager();

    void AddTexture(int textureID, const std::string &assetsPath, const std::string &fileName, bool pixelArt);
    void AddSpriteSheet(int sheetID, const std::string &assetsPath, const std::string &fileName, bool pixelArt);
    void AddFont(int fontID, const std::string &assetsPath, const std::string &fileName, float size);
    void AddSound(int soundID, const std::string &assetsPath, const std::string &fileName);
    void AddMusic(int musicID, const std::string &assetsPath, const std::string &fileName);

    int LoadSpriteSheets(const std::vector<int> &sheetIDs);

    SDL_Texture *GetTexture(int textureID);
    SpriteSheet *GetSpriteSheet(int sheetID);
    TTF_Font *GetFont(int fontID);
    Mix_Chunk *GetSound(int soundID);
    Mix_Music *GetMusic(int musicID);

    float GetFontInitialSize(int fontID) const;
    void InitSoundFXChannels(int startID, int endID);
    void SetSoundVolume(int soundID, float volume);
    void SetMusicVolume(float volume);
    void SetSoundFXVolume(float volume);
    void PlaySoundFX(int soundID, int loop = 0);
    void PlaySound(int soundID, int channelID, int loops = 0);
    void PlayMusic(int musicID, int loops = -1);
    void FadeInMusic(int musicID, int loops = -1, int ms = 500, double position = 0.0);
    void FadeOutMusic(int ms = 500);

    static void CreateIOStream(const std::string &path, SDL_IOStream **ioStream, void **buffer);
    static void DestroyIOStream(SDL_IOStream *ioStream, void *buffer);

    static void ObfuscateMem(void *memory, size_t size);
    static void RetriveMem(void *memory, size_t size);

private:
    class MusicData
    {
    public:
        MusicData(const std::string &path);
        ~MusicData();

        Mix_Music *GetMusic();

    private:
        std::string m_path;
        Mix_Music *m_music;
        SDL_IOStream *m_ioStream;
        void *m_ioStreamBuffer;
    };

    class FontData
    {
    public:
        FontData(const std::string &path, float size);
        ~FontData();

        TTF_Font *GetFont();

        std::string m_path;
        TTF_Font *m_font;
        float m_size;
        SDL_IOStream *m_ioStream;
        void *m_ioStreamBuffer;
    };

    class SoundData
    {

    public:
        SoundData(const std::string &path);
        ~SoundData();

        Mix_Chunk *GetSound();

    private:
        Mix_Chunk *m_chunk;
    };

    class SheetData
    {

    public:
        SheetData(const std::string &path, bool pixelArt);
        ~SheetData();

        SpriteSheet *GetSpriteSheet();

    private:
        SpriteSheet *m_sheet;
        std::string m_path;
        bool m_pixelArt;
    };

    class TextureData
    {

    public:
        TextureData(const std::string &path, bool pixelArt);
        ~TextureData();

        SDL_Texture *GetTexture();

    private:
        SDL_Texture *m_texture;
        std::string m_path;
        bool m_pixelArt;
    };

    std::map<int, SheetData *>   m_sheetMap;
    std::map<int, TextureData *> m_textureMap;
    std::map<int, FontData *>    m_fontMap;
    std::map<int, MusicData *>   m_musicMap;
    std::map<int, SoundData *>   m_soundMap;

    std::vector<int> m_sfxChannels;
    int m_sfxIndex;
};
