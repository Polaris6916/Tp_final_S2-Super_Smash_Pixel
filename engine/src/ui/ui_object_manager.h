/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"

class UIObject;

class UIObjectManager
{
public:
    UIObjectManager();
    UIObjectManager(UIObjectManager const&) = delete;
    UIObjectManager& operator=(UIObjectManager const&) = delete;
    ~UIObjectManager();

    void AddObject(UIObject *object);
    void DeleteObject(UIObject *object);
    void SetVisible(UIObject *object, bool visible);
    bool IsVisible(UIObject *object) const;
    void DeleteObjects();
    bool Contains(UIObject *object) const;

    void ClearVisibleObjects();
         
    void ProcessObjects();
    void ProcessVisibleObjects();

    std::set<UIObject *>::iterator begin();
    std::set<UIObject *>::iterator end();

    const std::vector<UIObject *>::iterator visibleObjectsBegin();
    const std::vector<UIObject *>::iterator visibleObjectsEnd();

    void PrintObjects() const;

private:
    friend class UIObject;
    int m_nextID;

    std::vector<UIObject *> m_visibleObjects;
    std::set<UIObject *> m_objects;
    std::set<UIObject *> m_visibleSet;
    std::set<UIObject *> m_toProcess;

    void PrintObjectsRec(UIObject *gameObject) const;
    static bool CompareUIObjects(const UIObject *objectA, const UIObject *objectB);
};

inline std::set<UIObject*>::iterator UIObjectManager::begin()
{
    return m_objects.begin();
}

inline std::set<UIObject*>::iterator UIObjectManager::end()
{
    return m_objects.end();
}

inline const std::vector<UIObject *>::iterator UIObjectManager::visibleObjectsBegin()
{
    return m_visibleObjects.begin();
}

inline const std::vector<UIObject *>::iterator UIObjectManager::visibleObjectsEnd()
{
    return m_visibleObjects.end();
}

inline bool UIObjectManager::IsVisible(UIObject *object) const
{
    auto it = m_visibleSet.find(object);
    return it != m_visibleSet.end();
}
