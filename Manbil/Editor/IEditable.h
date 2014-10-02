#pragma once

#include "EditorObject.h"


//An object that be edited. Describes how to build an editor for it.
class IEditable
{
public:

    virtual void BuildEditorElements(std::vector<EditorObjectPtr> & outElements) = 0;
};