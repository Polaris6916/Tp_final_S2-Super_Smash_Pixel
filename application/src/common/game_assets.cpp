/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "common/game_assets.h"
#include "common/game_common.h"

const Color Colors::Black = Color(0, 0, 0, 255);
const Color Colors::White = Color(255, 255, 255, 255);
const Color Colors::Red = Color(255, 0, 0, 255);
const Color Colors::Green = Color(0, 255, 0, 255);
const Color Colors::Blue = Color(0, 0, 255, 255);
const Color Colors::Yellow = Color(255, 255, 0, 255);
const Color Colors::Magenta = Color(255, 0, 255, 255);
const Color Colors::Cyan = Color(0, 255, 255, 255);

// Ajoutez vos couleurs personnalisées
const Color Colors::Gold = Color(225, 185, 143);
const Color Colors::DarkBlue = Color(10, 10, 25);

namespace assets
{
    struct TextureLoadInfo
    {
        TextureID textureID;
        const char *path;
    };

    struct SheetLoadInfo
    {
        SheetID sheetID;
        const char *path;
    };

    struct FontLoadInfo
    {
        FontID fontID;
        const char *path;
        float size;
    };

    struct SoundLoadInfo
    {
        SoundID soundID;
        const char *path;
    };

    struct MusicLoadInfo
    {
        MusicID soundID;
        const char *path;
    };

    struct AnimLoadInfo
    {
        AnimCategory category;
        AnimType type;
        SheetID sheetID;
        const char *groupName;
        float fps;
        int loopCount;
    };

    struct PlayerAnimLoadInfo
    {
        AnimType type;
        const char *groupName;
        float fps;
        int loopCount;
    };
}

void assets::InitAssets(AssetManager *assets)
{
    static const SheetLoadInfo sheetsToLoad[] = {
        // Stage
        { SHEET_TILESET_STAR_FIELDS, "background/tileset_star_fields.json" },
        // UI
        { SHEET_UI, "atlas/ui/ui_base.json" },
        { SHEET_UI_STAGE, "atlas/ui/ui_stage.json" },
        // Player
        { SHEET_FIRE_KNIGHT_0, "atlas/fighter/fire_knight_0.json" },
        { SHEET_FIRE_KNIGHT_1, "atlas/fighter/fire_knight_1.json" },
        { SHEET_WATER_PRIESTESS_0, "atlas/fighter/water_priestess_0.json" },
        { SHEET_WATER_PRIESTESS_1, "atlas/fighter/water_priestess_1.json" },
        { SHEET_LEAF_RANGER_0, "atlas/fighter/leaf_ranger_0.json" },
		{ SHEET_LEAF_RANGER_1, "atlas/fighter/leaf_ranger_1.json" },
		{ SHEET_METAL_BLADEKEEPER_0, "atlas/fighter/metal_bladekeeper_0.json" },
		{ SHEET_METAL_BLADEKEEPER_1, "atlas/fighter/metal_bladekeeper_1.json" },
        { SHEET_SHIELD, "atlas/Shield.json" },
        // Item
        { SHEET_ITEM_POTION, "atlas/item/potion.json" },
        { SHEET_ITEM_BOMB, "atlas/item/bomb.json" },
        { SHEET_ITEM_CRYSTAL, "atlas/item/crystal.json" },
        // Effects
        { SHEET_VFX_HIT, "atlas/vfx/hit_effect_01.json" },
        { SHEET_VFX_PARTICLES, "atlas/vfx/particles.json" },
        { SHEET_VFX_POWER, "atlas/vfx/power.json" },
        { SHEET_VFX_HEAL, "atlas/vfx/heal_effect.json" },
        { SHEET_VFX_POTION, "atlas/vfx/potion_effect.json" },
        { SHEET_VFX_EXPLOSION, "atlas/vfx/explosion.json" },
        // Dust & impact
        { SHEET_VFX_IMPACT_DUST, "atlas/dust/impact_smoke.json" },
        { SHEET_VFX_SIDE_IMPACT, "atlas/dust/side_impact.json" },
        { SHEET_VFX_BIG_DUST, "atlas/dust/big_smoke.json" },
        { SHEET_VFX_DASH_SMOKE_1, "atlas/dust/dash_smoke_1.json" },
        { SHEET_VFX_DASH_SMOKE_2, "atlas/dust/dash_smoke_2.json" },
        { SHEET_VFX_DUST_1, "atlas/dust/dust_1.json" },
        { SHEET_VFX_DUST_2, "atlas/dust/dust_2.json" },
        { SHEET_VFX_DUST_3, "atlas/dust/dust_3.json" },
        { SHEET_VFX_DUST_4, "atlas/dust/dust_4.json" },
        // Attack Player
		{ SHEET_ATTACK_LEAF_RANGER, "atlas/fighter/leaf_ranger_arrows.json" },
		{ SHEET_ATTACK_METAL_BLADEKEEPER, "atlas/fighter/metal_projectile.json" },
    };

    for (const auto &sheet : sheetsToLoad)
    {
        assets->AddSpriteSheet(sheet.sheetID, ASSETS_PATH, sheet.path, true);
    }
}

void assets::InitTextures(AssetManager * assets)
{
    static const TextureLoadInfo texturesToLoad[] = {
        // Stage
        { TEXTURE_STAR_FIELDS_0, "background/background_star_fields_0.dat" },
        { TEXTURE_STAR_FIELDS_1, "background/background_star_fields_1.dat" },
        { TEXTURE_STAR_FIELDS_2, "background/background_star_fields_2.dat" },
    };
    for (const auto &texture : texturesToLoad)
    {
        assets->AddTexture(texture.textureID, ASSETS_PATH, texture.path, true);
    }
}

void assets::InitFonts(AssetManager * assets)
{
    static const FontLoadInfo fontsToLoad[] = {
        { FONT_SMALL, "font/futile_pro.dat", 32 },
        { FONT_LARGE, "font/futile_pro.dat", 64 },
        { FONT_TEXT, "font/m5x7.dat", 48 },
        { FONT_NORMAL, "font/m5x7.dat", 48 },
        { FONT_TIME, "font/futile_pro.dat", 64 },
    };
    for (const auto &font : fontsToLoad)
    {
        assets->AddFont(font.fontID, ASSETS_PATH, font.path, font.size);
    }
}

void assets::InitSFX(AssetManager * assets)
{
    assets->InitSoundFXChannels(0, MIX_CHANNEL_COUNT - 2);
    static const SoundLoadInfo soundsToLoad[] = {
        // Attacf
        { SFX_SWORD_ATTACK_1, "sound_fx/attack/Sword Attack 1.dat" },
        { SFX_SWORD_ATTACK_2, "sound_fx/attack/Sword Attack 2.dat" },
        { SFX_SWORD_ATTACK_3, "sound_fx/attack/Sword Attack 3.dat" },
        { SFX_WHOOSH_1, "sound_fx/attack/WHOOSH_ARM_SWING_01.dat" },
        { SFX_WHOOSH_2, "sound_fx/attack/WHOOSH_AIRY_FLUTTER_01.dat" },
        // Hit
        { SFX_SWORD_HIT_A1, "sound_fx/hit/HIT_SHORT_04.dat" },
        { SFX_SWORD_HIT_A2, "sound_fx/hit/PUNCH_CLEAN_HEAVY_10.dat" },
        { SFX_SWORD_HIT_A3, "sound_fx/hit/PUNCH_DESIGNED_HEAVY_74.dat" },
        { SFX_SWORD_HIT_B1, "sound_fx/hit/PUNCH_PERCUSSIVE_HEAVY_08.dat" },
        { SFX_SWORD_HIT_B2, "sound_fx/hit/PUNCH_PERCUSSIVE_HEAVY_09.dat" },
        // Spell
        { SFX_FIRE_1, "sound_fx/spell/Fireball 1.dat" },
        { SFX_FIRE_2, "sound_fx/spell/Fireball 3.dat" },
        { SFX_EXPLOSION, "sound_fx/spell/EXPLDsgn_Explosion Impact_14.dat" },
        { SFX_WATER, "sound_fx/spell/22_Water_02.dat" },
        { SFX_ICE, "sound_fx/spell/13_Ice_explosion_01.dat" },
        // Move
        { SFX_JUMP_AIR, "sound_fx/move/30_Jump_03.dat" },
        { SFX_LAND, "sound_fx/move/Dirt Land.dat" },
        { SFX_JUMP_GROUND, "sound_fx/move/Dirt Jump.dat" },
        { SFX_ROLL, "sound_fx/move/Stone Jump.dat" },
        // UI
        { SFX_UI_CLICK, "sound_fx/ui/DM-CGS-21.dat" },
        { SFX_UI_ITEM, "sound_fx/ui/DM-CGS-40.dat" },
        { SFX_UI_SELECTED, "sound_fx/ui/DM-CGS-01.dat" },
        { SFX_UI_VALIDATE, "sound_fx/ui/DM-CGS-32.dat" },
        { SFX_UI_BACK, "sound_fx/ui/DM-CGS-16.dat" },
        { SFX_UI_PAUSE, "sound_fx/ui/DM-CGS-16.dat" },
        { SFX_UI_UNPAUSE, "sound_fx/ui/DM-CGS-16.dat" },
        { SFX_UI_COUNTDOWN, "sound_fx/ui/DM-CGS-49.dat" },

    };
    for (const auto &sound : soundsToLoad)
    {
        assets->AddSound(sound.soundID, ASSETS_PATH, sound.path);
    }
}

void assets::InitMusic(AssetManager * assets)
{
    static const MusicLoadInfo musicsToLoad[] = {
        // Attack
        { MUSIC_MENU, "music/TomMusic_Call_to_Adventure.dat" },
        { MUSIC_STAGE, "music/TomMusic_The_Wilderness.dat" },
    };
    for (const auto &music : musicsToLoad)
    {
        assets->AddMusic(music.soundID, ASSETS_PATH, music.path);
    }
}

void assets::InitSpriteAnimations(Scene *scene)
{
    SpriteAnimManager *animManager = scene->GetAnimManager();
    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = nullptr;
    const float attackFPS = 1.f / PLAYER_ATTACK_FRAME_TIME;

    static const AnimLoadInfo animationsToLoad[] = {
        { AnimCategory::ITEM, AnimType::POTION, SHEET_ITEM_POTION, "Potion", 10.f, -1 },
        { AnimCategory::ITEM, AnimType::BOMB_NORMAL, SHEET_ITEM_BOMB, "BombCharging", 4.f, -1 },
        { AnimCategory::ITEM, AnimType::BOMB_DANGER, SHEET_ITEM_BOMB, "BombDanger", 10.f, 3 },
        { AnimCategory::ITEM, AnimType::CRYSTAL, SHEET_ITEM_CRYSTAL, "Rotate", 20.f, -1 },
        //
        { AnimCategory::DEFAULT, AnimType::SHIELD, SHEET_SHIELD, "Shield", 20.f, -1 },
        //
        { AnimCategory::DUST, AnimType::SMALL_DUST_1, SHEET_VFX_DUST_3, "Effect", 15.f, 1 },
        { AnimCategory::DUST, AnimType::SMALL_DUST_2, SHEET_VFX_DUST_4, "Effect", 15.f, 1 },
        { AnimCategory::DUST, AnimType::ROLL_DUST, SHEET_VFX_DASH_SMOKE_2, "Effect", 15.f, 1 },
        { AnimCategory::DUST, AnimType::SKID_DUST, SHEET_VFX_DUST_2, "Effect", 15.f, 1 },
        { AnimCategory::DUST, AnimType::JUMP_DUST, SHEET_VFX_BIG_DUST, "Effect", 20.f, 1 },
        { AnimCategory::DUST, AnimType::LAND_DUST, SHEET_VFX_IMPACT_DUST, "Effect", 15.f, 1 },
        { AnimCategory::DUST, AnimType::SIDE_IMPACT, SHEET_VFX_SIDE_IMPACT, "Effect", 20.f, 1 },
        //
        { AnimCategory::VFX, AnimType::SQUARE_PARTICULE, SHEET_VFX_PARTICLES, "Square", 1.f, 0 },
        { AnimCategory::VFX, AnimType::CHARGED_PARTICULE, SHEET_VFX_PARTICLES, "VerticalRay", 1.f, 0 },
        { AnimCategory::VFX, AnimType::POWER_AURA, SHEET_VFX_POWER, "Power", 15.f, 1 },
        { AnimCategory::VFX, AnimType::POTION, SHEET_VFX_POTION, "Effect", 15.f, 1 },
        { AnimCategory::VFX, AnimType::HEAL, SHEET_VFX_HEAL, "Effect", 15.f, 1 },
        { AnimCategory::VFX, AnimType::EXPLOSION, SHEET_VFX_EXPLOSION, "Effect", 15.f, 1 },
        { AnimCategory::VFX, AnimType::ARROW_1, SHEET_ATTACK_LEAF_RANGER, "ArrowThorns", 15.f, -1},
        { AnimCategory::VFX, AnimType::ARROW_2_DEBUT, SHEET_ATTACK_LEAF_RANGER, "ArrowHit", 15.f, 1},
        { AnimCategory::VFX, AnimType::ARROW_2_FIN, SHEET_ATTACK_LEAF_RANGER, "ArrowShower", 15.f, 1},
        { AnimCategory::VFX, AnimType::ARROW_3, SHEET_ATTACK_LEAF_RANGER, "SimpleHit", 5.f, 1},
        { AnimCategory::VFX, AnimType::ARROW_SMASH, SHEET_ATTACK_LEAF_RANGER, "BeamEnd", 15.f, 1},
        { AnimCategory::VFX, AnimType::ARROW_AIR, SHEET_ATTACK_LEAF_RANGER, "ThornsHit", 15.f, 1},
        { AnimCategory::VFX, AnimType::KNIFE, SHEET_ATTACK_METAL_BLADEKEEPER, "KnifeHit", 15.f, 1},
        { AnimCategory::VFX, AnimType::BLADE_1, SHEET_ATTACK_METAL_BLADEKEEPER, "BladesA", 15.f, 1},
        { AnimCategory::VFX, AnimType::BLADE_2, SHEET_ATTACK_METAL_BLADEKEEPER, "BladesB1", 15.f, 1},
        { AnimCategory::VFX, AnimType::BLADE_3, SHEET_ATTACK_METAL_BLADEKEEPER, "BladesB2", 5.f, 1},
        { AnimCategory::VFX, AnimType::BLADE_4, SHEET_ATTACK_METAL_BLADEKEEPER, "BladesB3", 15.f, 1},
        { AnimCategory::VFX, AnimType::BLADE_5, SHEET_ATTACK_METAL_BLADEKEEPER, "BladesB4", 15.f, 1},
    };

    for (const auto& animToLoad : animationsToLoad)
    {
        auto* spriteSheet = assets->GetSpriteSheet(animToLoad.sheetID);
        AssertNew(spriteSheet);

        SpriteGroup *spriteGroup = spriteSheet->GetGroup(animToLoad.groupName);
        AssertNew(spriteGroup);

        uint32_t animID = AnimID_Make(animToLoad.category, animToLoad.type);
        animManager->AddSpriteAnim(animID, spriteGroup);
        SpriteAnim *spriteAnim = animManager->GetSpriteAnim(animID);
        AssertNew(spriteAnim);
        spriteAnim->SetCycleCount(animToLoad.loopCount);
        spriteAnim->SetFPS(animToLoad.fps);
    }

    assets::InitAnimations_FireKnight(assets, animManager);
    assets::InitAnimations_WaterPriestress(assets, animManager);
    assets::InitAnimations_LeafRanger(assets, animManager);
    assets::InitAnimations_Metal_Bladekeeper(assets, animManager);
}

void assets::InitAnimations_FireKnight(AssetManager *assets, SpriteAnimManager *animManager)
{
    const float attackFPS = 1.f / PLAYER_ATTACK_FRAME_TIME;

    // TODO 1 - Corrigez l'animation RUN en regardant son nom dans le fichier
    //        assets/atlas/fighter/fire_knight_0.json
    //      - Donnez la bonne valeur pour les FPS
    //      - Donnez la bonne valeur pour le nombre de cycles (-1 pour se répéter indéfiniment)

    static const PlayerAnimLoadInfo animationsToLoad[] = {
        { AnimType::IDLE, "Idle", 15.f, -1 },
        { AnimType::RUN, "Running", 5.f, -1 },
        { AnimType::SKID, "Skidding", 15.f, -1 },
        { AnimType::ROLL, "Roll", 22.f, 1 },
        { AnimType::JUMP_UP, "JumpUp", 13.f, 1 },
        { AnimType::JUMP_TOP, "JumpTop", 20.f, 1 },
        { AnimType::JUMP_DOWN, "JumpDown", 13.f, -1 },
        { AnimType::ATTACK_1, "Attack1", attackFPS, 1 },
        { AnimType::ATTACK_2, "Attack2", attackFPS, 1 },
        { AnimType::ATTACK_3, "Attack3", attackFPS, 1 },
        { AnimType::ATTACK_1_END, "Attack1End", attackFPS, 1 },
        { AnimType::ATTACK_2_END, "Attack2End", attackFPS, 1 },
        { AnimType::ATTACK_AIR, "AttackAir", attackFPS, 1 },
        { AnimType::SMASH_START, "SmashStart", 20.f, 1 },
        { AnimType::SMASH_HOLD, "SmashHold", 15.f, -1 },
        { AnimType::SMASH_RELEASE, "SmashRelease", attackFPS, 1 },
        { AnimType::DEFEND_START, "StartDefend", 20.f, 1 },
        { AnimType::DEFEND, "Defend", 20.f, -1 },
        { AnimType::TAKE_HIT, "TakeHit", 25.f, 1 },
        { AnimType::PLAYER_INTRO, "Introduction", 25.f, 1 },
    };
    static const AnimCategory categories[] = { AnimCategory::FIRE_KNIGHT_0, AnimCategory::FIRE_KNIGHT_1 };
    static const int sheetIDs[] = { SHEET_FIRE_KNIGHT_0, SHEET_FIRE_KNIGHT_1 };

    for (int i = 0; i < 2; i++)
    {
        for (const auto& animToLoad : animationsToLoad)
        {
            auto* spriteSheet = assets->GetSpriteSheet(sheetIDs[i]);
            AssertNew(spriteSheet);

            SpriteGroup *spriteGroup = spriteSheet->GetGroup(animToLoad.groupName);
            AssertNew(spriteGroup);

            uint32_t animID = AnimID_Make(categories[i], animToLoad.type);
            animManager->AddSpriteAnim(animID, spriteGroup);
            SpriteAnim *spriteAnim = animManager->GetSpriteAnim(animID);
            AssertNew(spriteAnim);
            spriteAnim->SetCycleCount(animToLoad.loopCount);
            spriteAnim->SetFPS(animToLoad.fps);
        }
    }
}

void assets::InitAnimations_WaterPriestress(AssetManager *assets, SpriteAnimManager *animManager)
{
    const float attackFPS = 1.f / PLAYER_ATTACK_FRAME_TIME;

    static const PlayerAnimLoadInfo animationsToLoad[] = {
        { AnimType::IDLE, "Idle", 15.f, -1 },
        { AnimType::RUN, "Running", 5.f, -1 },
        { AnimType::SKID, "Skidding", 15.f, -1 },
        { AnimType::ROLL, "Roll", 22.f, 1 },
        { AnimType::JUMP_UP, "JumpUp", 13.f, 1 },
        { AnimType::JUMP_TOP, "JumpTop", 20.f, 1 },
        { AnimType::JUMP_DOWN, "JumpDown", 13.f, -1 },
        { AnimType::ATTACK_1, "Attack1", attackFPS, 1 },
        { AnimType::ATTACK_2, "Attack2", attackFPS, 1 },
        { AnimType::ATTACK_3, "Attack3", attackFPS, 1 },
        { AnimType::ATTACK_1_END, "Attack1End", attackFPS, 1 },
        { AnimType::ATTACK_2_END, "Attack2End", attackFPS, 1 },
        { AnimType::ATTACK_AIR, "AttackAir", attackFPS, 1 },
        { AnimType::SMASH_START, "SmashStart", 20.f, 1 },
        { AnimType::SMASH_HOLD, "SmashHold", 15.f, -1 },
        { AnimType::SMASH_RELEASE, "SmashRelease", attackFPS, 1 },
        { AnimType::DEFEND_START, "StartDefend", 20.f, 1 },
        { AnimType::DEFEND, "Defend", 20.f, -1 },
        { AnimType::TAKE_HIT, "TakeHit", 25.f, 1 },
        { AnimType::PLAYER_INTRO, "Introduction", 25.f, 1 },
    };
    static const AnimCategory categories[] = { AnimCategory::WATER_PRIESTESS_0, AnimCategory::WATER_PRIESTESS_1 };
    static const int sheetIDs[] = { SHEET_WATER_PRIESTESS_0, SHEET_WATER_PRIESTESS_1 };

    for (int i = 0; i < 2; i++)
    {
        for (const auto& animToLoad : animationsToLoad)
        {
            auto* spriteSheet = assets->GetSpriteSheet(sheetIDs[i]);
            AssertNew(spriteSheet);

            SpriteGroup *spriteGroup = spriteSheet->GetGroup(animToLoad.groupName);
            AssertNew(spriteGroup);

            uint32_t animID = AnimID_Make(categories[i], animToLoad.type);
            animManager->AddSpriteAnim(animID, spriteGroup);
            SpriteAnim *spriteAnim = animManager->GetSpriteAnim(animID);
            AssertNew(spriteAnim);
            spriteAnim->SetCycleCount(animToLoad.loopCount);
            spriteAnim->SetFPS(animToLoad.fps);
        }
    }
}

void assets::InitAnimations_LeafRanger(AssetManager* assets, SpriteAnimManager* animManager)
{
    const float attackFPS = 1.f / PLAYER_ATTACK_FRAME_TIME;

    static const PlayerAnimLoadInfo animationsToLoad[] = {
        { AnimType::IDLE, "Idle", 15.f, -1 },
        { AnimType::RUN, "Running", 5.f, -1 },
        { AnimType::SKID, "Skidding", 15.f, -1 },
        { AnimType::ROLL, "Roll", 22.f, 1 },
        { AnimType::JUMP_UP, "JumpUp", 13.f, 1 },
        { AnimType::JUMP_TOP, "JumpTop", 20.f, 1 },
        { AnimType::JUMP_DOWN, "JumpDown", 13.f, -1 },
        { AnimType::ATTACK_1, "Attack1", attackFPS, 1 },
        { AnimType::ATTACK_2, "Attack2", attackFPS, 1 },
        { AnimType::ATTACK_3, "Attack3", attackFPS, 1 },
        { AnimType::ATTACK_1_END, "Attack1End", attackFPS, 1 },
        { AnimType::ATTACK_2_END, "Attack2End", attackFPS, 1 },
        { AnimType::ATTACK_AIR, "AttackAir", attackFPS, 1 },
        { AnimType::SMASH_START, "SmashStart", 20.f, 1 },
        { AnimType::SMASH_HOLD, "SmashHold", 15.f, -1 },
        { AnimType::SMASH_RELEASE, "SmashRelease", attackFPS, 1 },
        { AnimType::DEFEND_START, "StartDefend", 20.f, 1 },
        { AnimType::DEFEND, "Defend", 20.f, -1 },
        { AnimType::TAKE_HIT, "TakeHit", 25.f, 1 },
        { AnimType::PLAYER_INTRO, "Introduction", 25.f, 1 },
    };
    static const AnimCategory categories[] = { AnimCategory::LEAF_RANGER_0, AnimCategory::LEAF_RANGER_1 };
    static const int sheetIDs[] = { SHEET_LEAF_RANGER_0, SHEET_LEAF_RANGER_1 };

    for (int i = 0; i < 2; i++)
    {
        for (const auto& animToLoad : animationsToLoad)
        {
            auto* spriteSheet = assets->GetSpriteSheet(sheetIDs[i]);
            AssertNew(spriteSheet);

            SpriteGroup* spriteGroup = spriteSheet->GetGroup(animToLoad.groupName);
            AssertNew(spriteGroup);

            uint32_t animID = AnimID_Make(categories[i], animToLoad.type);
            animManager->AddSpriteAnim(animID, spriteGroup);
            SpriteAnim* spriteAnim = animManager->GetSpriteAnim(animID);
            AssertNew(spriteAnim);
            spriteAnim->SetCycleCount(animToLoad.loopCount);
            spriteAnim->SetFPS(animToLoad.fps);
        }
    }
}

void assets::InitAnimations_Metal_Bladekeeper(AssetManager* assets, SpriteAnimManager* animManager)
{
    const float attackFPS = 1.f / PLAYER_ATTACK_FRAME_TIME;

    static const PlayerAnimLoadInfo animationsToLoad[] = {
        { AnimType::IDLE, "Idle", 15.f, -1 },
        { AnimType::RUN, "Running", 5.f, -1 },
        { AnimType::SKID, "Skidding", 15.f, -1 },
        { AnimType::ROLL, "Roll", 22.f, 1 },
        { AnimType::JUMP_UP, "JumpUp", 13.f, 1 },
        { AnimType::JUMP_TOP, "JumpTop", 20.f, 1 },
        { AnimType::JUMP_DOWN, "JumpDown", 13.f, -1 },
        { AnimType::ATTACK_1, "Attack1", attackFPS, 1 },
        { AnimType::ATTACK_2, "Attack2", attackFPS, 1 },
        { AnimType::ATTACK_3, "Attack3", attackFPS, 1 },
        { AnimType::ATTACK_1_END, "Attack1End", attackFPS, 1 },
        { AnimType::ATTACK_2_END, "Attack2End", attackFPS, 1 },
        { AnimType::ATTACK_AIR, "AttackAir", attackFPS, 1 },
        { AnimType::SMASH_START, "SmashStart", 20.f, 1 },
        { AnimType::SMASH_HOLD, "SmashHold", 15.f, -1 },
        { AnimType::SMASH_RELEASE, "SmashRelease", attackFPS, 1 },
        { AnimType::DEFEND_START, "DefendStart", 20.f, 1 },
        { AnimType::DEFEND, "Defend", 20.f, -1 },
        { AnimType::TAKE_HIT, "TakeHit", 25.f, 1 },
        { AnimType::PLAYER_INTRO, "Introduction", 25.f, 1 },
    };
    static const AnimCategory categories[] = { AnimCategory::METAL_BLADEKEEPER_0, AnimCategory::METAL_BLADEKEEPER_1 };
    static const int sheetIDs[] = { SHEET_METAL_BLADEKEEPER_0, SHEET_METAL_BLADEKEEPER_1 };

    for (int i = 0; i < 2; i++)
    {
        for (const auto& animToLoad : animationsToLoad)
        {
            auto* spriteSheet = assets->GetSpriteSheet(sheetIDs[i]);
            AssertNew(spriteSheet);

            SpriteGroup* spriteGroup = spriteSheet->GetGroup(animToLoad.groupName);
            AssertNew(spriteGroup);

            uint32_t animID = AnimID_Make(categories[i], animToLoad.type);
            animManager->AddSpriteAnim(animID, spriteGroup);
            SpriteAnim* spriteAnim = animManager->GetSpriteAnim(animID);
            AssertNew(spriteAnim);
            spriteAnim->SetCycleCount(animToLoad.loopCount);
            spriteAnim->SetFPS(animToLoad.fps);
        }
    }
}

std::string assets::AnimIDToString(AnimID animID)
{
    return std::string(magic_enum::enum_name(AnimID_GetCategory(animID)))
        + "::" + std::string(magic_enum::enum_name(AnimID_GetType(animID)));
}

void game::UpdateFontSize(Scene *scene)
{
    SDL_Rect viewport = { 0 };
    SDL_GetRenderViewport(g_renderer, &viewport);
    AssetManager *assets = scene->GetAssetManager();

    const float scale = viewport.w / 1920.0f;
    for (int i = 0; i < static_cast<int>(FontID::FONT_COUNT); i++)
    {
        FontID fontID = static_cast<FontID>(i);
        TTF_Font *font = assets->GetFont(fontID);
        if (font == nullptr) continue;

        float initialSize = assets->GetFontInitialSize(fontID);

        TTF_SetFontSize(font, roundf(initialSize * scale));
    }
}
