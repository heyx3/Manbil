#pragma once

#include "EditorObject.h"


//An object that be edited. Describes how to build an editor for it.
class IEditable
{
public:

    //Returns an error message, or the empty string if everything went fine.
    virtual std::string BuildEditorElements(std::vector<EditorObjectPtr> & outElements,
                                            EditorMaterialSet & materialSet) = 0;
};