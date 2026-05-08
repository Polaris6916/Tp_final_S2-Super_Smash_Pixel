/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_object.h"

class UIGridLayout : public UIObject
{
public:
    UIGridLayout(Scene *scene, int rowCount, int columnCount);
    void SetPadding(float paddingX, float paddingY);
    void SetPadding(float padding);
    void SetSpacing(float spacing);
    void SetAnchor(Anchor anchor);

    void AddObject(
        UIObject *object, int rowIdx, int columnIdx,
        int rowSpan = 1, int columnSpan = 1);
    void RemoveObject(UIObject *object);

    void SetRowSize(int index, float size);
    void SetRowSize(float size);
    void SetColumnSize(int index, float size);
    void SetColumnSize(float size);
    void SetRowSpacing(int index, float spacing);
    void SetRowSpacing(float spacing);
    void SetColumnSpacing(int index, float spacing);
    void SetColumnSpacing(float spacing);

    virtual void Update() override;
    virtual void DrawGizmos() override;
    virtual void DrawImGui() override;

protected:

    struct GridCell
    {
        GridCell(int rowIdx, int colIdx, int rowSpan, int colSpan):
            rowIdx(rowIdx), colIdx(colIdx),
            rowSpan(rowSpan), colSpan(colSpan)
        {}
        size_t rowIdx;
        size_t colIdx;
        size_t rowSpan;
        size_t colSpan;
    };

    int m_rowCount;
    int m_colCount;
    b2Vec2 m_padding;
    std::vector<float> m_rowSizes;
    std::vector<float> m_colSizes;
    std::vector<float> m_rowSpacings;
    std::vector<float> m_colSpacings;
    std::vector<float> m_rowOffsets;
    std::vector<float> m_colOffsets;
    std::map<UIObject *, GridCell> m_gridMap;
    Anchor m_anchor;

private:
    void UpdateOffsets();
};

inline void UIGridLayout::SetPadding(float paddingX, float paddingY)
{
    m_padding = { paddingX, paddingY };
    Update();
}

inline void UIGridLayout::SetPadding(float padding)
{
    m_padding = { padding, padding };
    Update();
}

inline void UIGridLayout::SetAnchor(Anchor anchor)
{
    m_anchor = anchor;
    Update();
}
