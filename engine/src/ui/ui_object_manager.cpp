/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_object_manager.h"
#include "ui/ui_object.h"

//#define DEBUG_OBJECT_ON_DELETE

UIObjectManager::UIObjectManager() :
    m_objects(), m_toProcess(), m_visibleObjects(), m_nextID(0)
{
}

UIObjectManager::~UIObjectManager()
{
    assert(m_objects.size() == 0);
}

void UIObjectManager::AddObject(UIObject *object)
{
    // Teste si l'objet est déjà présent
    if (Contains(object)) return;

    // Teste si l'objet est en attente de Start()
    if (m_toProcess.find(object) != m_toProcess.end()) return;

    object->AddFlags(UIObject::Flag::TO_START);
    object->m_objectID = m_nextID++;

    m_toProcess.insert(object);
}

void UIObjectManager::DeleteObject(UIObject *object)
{
    m_toProcess.insert(object);

    object->AddFlags(UIObject::Flag::TO_DELETE);

    for (UIObject *child : object->GetChildren())
    {
        DeleteObject(child);
    }
}

void UIObjectManager::SetVisible(UIObject *object, bool visible)
{
    if (visible)
        m_visibleSet.insert(object);
    else
        m_visibleSet.erase(object);
}

void UIObjectManager::DeleteObjects()
{
    ProcessObjects();
    for (UIObject *gameObject : m_objects)
    {
        delete gameObject;
    }
    m_objects.clear();
    m_toProcess.clear();
    m_visibleObjects.clear();
    m_visibleSet.clear();
}

bool UIObjectManager::Contains(UIObject *object) const
{
    if (object == nullptr) return false;
    return m_objects.find(object) != m_objects.end();
}

void UIObjectManager::ClearVisibleObjects()
{
    m_visibleObjects.clear();
}

void UIObjectManager::ProcessObjects()
{
    std::set<UIObject *> toDelete;
    std::vector<UIObject *> toProcessCopy(m_toProcess.begin(), m_toProcess.end());

    // Remet à zéro le conteneur
    m_toProcess.clear();
    m_visibleSet.clear();

    // Trie les objets selon leurs profondeurs
    std::sort(toProcessCopy.begin(), toProcessCopy.end(), CompareUIObjects);

    for (auto &gameObject : toProcessCopy)
    {
        // START
        if (gameObject->TestFlag(UIObject::Flag::TO_START))
        {
            // Insertion dans la liste des objets
            m_objects.insert(gameObject);

            gameObject->SubFlags(UIObject::Flag::TO_START);
            gameObject->Start();
        }

        // DELETE
        if (gameObject->TestFlag(UIObject::Flag::TO_DELETE))
        {
            toDelete.insert(gameObject);
        }

        // DEBUG
    #ifdef DEBUG_OBJECT_ON_DELETE
        if (gameObject->TestFlag(UIObject::Flag::DEBUG_DELETED))
        {
            assert(false);
        }
    #endif
    }

    for (auto gameObject : toDelete)
    {
        gameObject->OnDelete();
    }
    for (auto gameObject : toDelete)
    {
        gameObject->SetParent(nullptr);
    }
    for (auto &gameObject : toDelete)
    {
        // On détruit toutes les références de l'objet
        m_toProcess.erase(gameObject);
        m_objects.erase(gameObject);
        m_visibleSet.erase(gameObject);

    #ifdef DEBUG_OBJECT_ON_DELETE
        gameObject->AddFlags(UIObject::Flag::DEBUG_DELETED);
    #else
        delete gameObject;
    #endif
    }
    m_visibleObjects.clear();
}

void UIObjectManager::PrintObjects() const
{
    // Trie les objets hierarchiquement
    std::cout << "Scene Game Objects:" << std::endl;
    for (UIObject *gameObject : m_objects)
    {
        if (gameObject->GetParent() != nullptr) continue;
        PrintObjectsRec(gameObject);
    }
}

void UIObjectManager::PrintObjectsRec(UIObject *gameObject) const
{
    for (int i = 0; i < gameObject->GetDepth(); i++)
        std::cout << std::string("    ");

    std::cout << "[" << gameObject->GetDepth() << "] ";

    UIObject *uiObject = dynamic_cast<UIObject *>(gameObject);
    if (uiObject && uiObject->IsEnabled() == false)
    {
        std::cout << "[UI DISABLED] ";
    }

    std::cout << gameObject->GetName() << " "
        << "(ID=" << gameObject->m_objectID << ")"
        << std::endl;

    for (UIObject *child : gameObject->GetChildren())
    {
        PrintObjectsRec(child);
    }
}

bool UIObjectManager::CompareUIObjects(const UIObject *objectA, const UIObject *objectB)
{
    if (objectA->GetLayer() == objectB->GetLayer())
    {
        if (objectA->GetDepth() == objectB->GetDepth())
        {
            return objectA->GetID() < objectB->GetID();
        }
        return objectA->GetDepth() < objectB->GetDepth();
    }
    return objectA->GetLayer() < objectB->GetLayer();
}

void UIObjectManager::ProcessVisibleObjects()
{
    m_visibleObjects.clear();
    for (UIObject *gameObject : m_visibleSet)
    {
        m_visibleObjects.push_back(gameObject);
    }
    
    // Trie les objets selon leurs layers
    std::sort(m_visibleObjects.begin(), m_visibleObjects.end(), CompareUIObjects);
}
