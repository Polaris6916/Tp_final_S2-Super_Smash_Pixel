/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"

/// @ingroup Timer
/// @brief Structure représentant un chronomètre.
class Timer
{
public:
    Timer();
    Timer(Timer const&) = delete;
    Timer& operator=(Timer const&) = delete;

    /// @brief Lance le timer.
    void Start();

    /// @brief Met à jour le timer en fonction du temps actuel.
    /// Cette fonction est typiquement appelée à chaque tour de la boucle de rendu.
    void Update();
    void Update(float deltaTime);
    void Update(Uint64 deltaTimeMS);

    void SetMaximumDeltaTime(float maxDelta);

    /// @brief Définit le facteur d'échelle de temps appliqué à un timer.
    /// Si l'échelle vaut 0.5f, le temps s'écoule deux fois moins rapidement.
    /// @param scale le facteur d'échelle.
    void SetTimeScale(float scale);

    /// @brief Renvoie le facteur d'échelle de temps d'un timer.
    /// @return Le facteur d'échelle de temps d'un timer.
    float GetTimeScale() const;

    /// @brief Renvoie l'écart de temps (en secondes) entre les deux derniers
    /// appels à la fonction Timer_update().
    /// @return L'écart de temps entre les deux dernières mises à jour.
    float GetDelta() const;

    /// @brief Renvoie l'écart de temps (en secondes) entre les deux derniers
    /// appels à la fonction Timer_update().
    /// Cette méthode ne tient pas compte de l'échelle de temps du timer.
    /// @return L'écart de temps entre les deux dernières mises à jour
    /// (sans échelle de temps).
    float GetUnscaledDelta() const;

    /// @brief Renvoie l'écart de temps (en secondes) entre le lancement du timer
    /// avec Timer_start() et le dernier appel à Timer_update().
    /// @return Le nombre de secondes écoulées depuis le lancement du timer et la dernière mise à jour.
    float GetElapsed() const;

    /// @brief Renvoie l'écart de temps (en secondes) entre le lancement du timer
    /// avec Timer_start() et le dernier appel à Timer_update().
    /// Cette méthode en tient pas compte de l'échelle de temps du timer.
    /// @return Le nombre de secondes écoulées depuis le lancement du timer et la
    /// dernière mise à jour (sans échelle de temps).
    float GetUnscaledElapsed() const;

    /// @brief Renvoie l'écart de temps (en millisecondes) entre les deux derniers
    /// appels à la fonction Timer_update().
    /// @return L'écart de temps entre les deux dernières mises à jour.
    Uint64 GetDeltaMS() const;

    /// @brief Renvoie l'écart de temps (en millisecondes) entre les deux derniers
    /// appels à la fonction Timer_update().
    /// Cette méthode ne tient pas compte de l'échelle de temps du timer.
    /// @return L'écart de temps entre les deux dernières mises à jour
    /// (sans échelle de temps).
    Uint64 GetUnscaledDeltaMS() const;

    /// @brief Renvoie l'écart de temps (en millisecondes) entre le lancement du timer
    /// avec Timer_start() et le dernier appel à Timer_update().
    /// @return Le nombre de millisecondes écoulées depuis le lancement du timer et la dernière mise à jour.
    Uint64 GetElapsedMS() const;

    /// @brief Renvoie l'écart de temps (en millisecondes) entre le lancement du timer
    /// avec Timer_start() et le dernier appel à Timer_update().
    /// Cette méthode en tient pas compte de l'échelle de temps du timer.
    /// @return Le nombre de millisecondes écoulées depuis le lancement du timer et la
    /// dernière mise à jour (sans échelle de temps).
    Uint64 GetUnscaledElapsedMS() const;

protected:
    /// @brief Temps de départ.
    /// Exprimé en millisecondes.
    Uint64 m_startTime;

    /// @brief Temps du dernier appel à Timer_update().
    /// Exprimé en millisecondes.
    Uint64 m_currentTime;

    /// @brief Temps de l'avant dernier appel à Timer_update().
    /// Exprimé en millisecondes.
    Uint64 m_previousTime;

    /// @brief Ecart entre les deux derniers appels à Timer_update().
    /// Ce membre est affecté par le facteur d'échelle.
    /// Exprimé en millisecondes.
    Uint64 m_delta;

    /// @brief Ecart entre les deux derniers appels à Timer_update().
    /// Ce membre n'est pas affecté par le facteur d'échelle.
    /// Exprimé en millisecondes.
    Uint64 m_unscaledDelta;

    /// @brief Facteur d'échelle appliqué au temps.
    float m_scale;

    /// @brief Ecart de temps maximum entre deux appels à Timer_update().
    /// Exprimé en millisecondes.
    Uint64 m_maxDelta;

    /// @brief Ecart entre le lancement du timer Timer_start()
    /// et le dernier appel à Timer_update().
    /// Ce membre est affecté par le facteur d'échelle.
    /// Exprimé en millisecondes.
    Uint64 m_elapsed;

    /// @brief Ecart entre le lancement du timer Timer_start()
    /// et le dernier appel à Timer_update().
    /// Ce membre n'est pas affecté par le facteur d'échelle.
    /// Exprimé en millisecondes.
    Uint64 m_unscaledElapsed;
};

inline void Timer::SetMaximumDeltaTime(float maxDelta)
{
    m_maxDelta = (Uint64)(maxDelta * 1000.f);
}

inline void Timer::SetTimeScale(float scale)
{
    m_scale = scale;
}

inline float Timer::GetDelta() const
{
    return (float)m_delta / 1000.f;
}

inline float Timer::GetTimeScale() const
{
    return m_scale;
};

inline float Timer::GetUnscaledDelta() const
{
    return (float)m_unscaledDelta / 1000.f;
}

inline float Timer::GetElapsed() const
{
    return (float)m_elapsed / 1000.f;
}

inline float Timer::GetUnscaledElapsed() const
{
    return (float)m_unscaledElapsed / 1000.f;
}

inline Uint64 Timer::GetDeltaMS() const
{
    return m_delta;
}

inline Uint64 Timer::GetUnscaledDeltaMS() const
{
    return m_unscaledElapsed;
}

inline Uint64 Timer::GetElapsedMS() const
{
    return m_elapsed;
}

inline Uint64 Timer::GetUnscaledElapsedMS() const
{
    return m_unscaledElapsed;
}

inline void Timer::Update(float deltaTime)
{
    Update((Uint64)(deltaTime * 1000.f));
}
