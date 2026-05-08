/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "utils/timer.h"

#define MIX_CHANNEL_COUNT 16
class AssetManager;

/// @brief Temps global du jeu.
extern Timer *g_time;

/// @brief Fenêtre du jeu.
extern SDL_Window *g_window;

/// @brief Moteur de rendu du jeu.
extern SDL_Renderer *g_renderer;

/// @brief Moteur de rendu des textes.
extern TTF_TextEngine *g_textEngine;

struct BodyUserData
{
    BodyUserData(entt::entity e) : entity(e) {}
    entt::entity entity;
};

namespace game
{
    float GetViewportScale();

    /// @brief Initialise les librairies utilisées par le jeu.
    /// @param sdlFlags les flags pour la librairie SDL.
    /// @param mixFlags les flags pour la librairie SDL Mixer.
    SDL_AppResult Init(int sdlFlags, int mixFlags);

    /// @brief Crée la fenêtre du jeu.
    /// @param width largeur de la fenêtre.
    /// @param height hauteur de la fenêtre.
    /// @param title la titre de la fenêtre.
    /// @param flags les flags SDL.
    SDL_AppResult CreateWindow(int width, int height, const std::string &title, Uint32 flags);

    /// @brief Crée le moteur de rendu.
    /// @param width largeur logique du rendu.
    /// @param height hauteur logique du rendu.
    SDL_AppResult CreateRenderer(int width, int height);

    /// @brief Détruit le moteur de rendu du jeu.
    void DestroyRenderer();

    /// @brief Détruit la fenêtre du jeu.
    void DestroyWindow();

    /// @brief Quitte les librairies utilisées par le jeu.
    void Quit();

    /// @brief Définit le volume de la musique.
    /// @param volume le volume de la musique (entre 0 et 1).
    void SetMusicVolume(float volume);

    /// @brief Définit le volume des cannaux utilisés pour les effets sonores.
    /// @param volume le volume du son (entre 0 et 1).
    void SetFXChannelsVolume(float volume);
};

/// @brief Renvoie la largeur logique du rendu.
/// @return La largeur logique du rendu.
int Game_getWidth();

/// @brief Renvoie la hauteur logique du rendu.
/// @return La hauteur logique du rendu.
int Game_getHeight();
/// @brief Définit le volume d'un son répertorié dans le gestionnaire des assets.
/// @param assets le gestionnaire des assets.
/// @param soundID l'identifiant du son.
/// @param volume le volume du son (entre 0 et 1).
void Game_setSoundVolume(AssetManager *assets, int soundID, float volume);


/// @brief Joue un effet sonore.
/// @param assets le gestionnaire des assets.
/// @param soundID l'identifiant du son.
void Game_playSoundFX(AssetManager *assets, int soundID);

/// @brief Joue une musique.
/// @param assets le gestionnaire des assets.
/// @param musicID l'identifiant de la musique.
void Game_playMusic(AssetManager *assets, int musicID);

/// @brief Définit la couleur du moteur de rendu utilisée par les opérations draw ou fill.
/// @param color la couleur (le paramètre alpha est ignoré).
/// @param alpha l'opacité (0 pour transparant, 255 pour opaque).
void Game_setRenderDrawColor(SDL_Color color, Uint8 alpha);

void Memcpy(void * const dst, size_t dstSize, const void *src, size_t srcSize);
char *Strdup(const char *src);
