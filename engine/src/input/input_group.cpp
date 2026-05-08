/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "input/input_group.h"

InputGroup::InputGroup()
    : m_enabled(true)
{
}

InputGroup::~InputGroup()
{
}

void InputGroup::OnPreEventProcess()
{
}

void InputGroup::OnEventProcess(SDL_Event evt)
{
}

void InputGroup::OnPostEventProcess()
{
}

void InputGroup::Reset()
{
}
