/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/title/ui_title_background.h"

UITitleBackground::UITitleBackground(Scene *scene)
    : UIObject(scene)
{
    SetName("Title background");
    SetLayer(LAYER_UI_BACKGROUND);
}

UITitleBackground::~UITitleBackground()
{
}

void UITitleBackground::Update()
{
    UIObject::Update();
    SetVisible(true);
}

void UITitleBackground::Render()
{
    AssetManager *assets = m_scene->GetAssetManager();
    TextureID textureIDs[] = {
        TEXTURE_STAR_FIELDS_0,
        TEXTURE_STAR_FIELDS_1,
        TEXTURE_STAR_FIELDS_2
    };
    for (auto textureID : textureIDs)
    {
        SDL_Texture *texture = assets->GetTexture(textureID);
        assert(texture);
        SDL_RenderTexture(g_renderer, texture, NULL, NULL);
    }
}
