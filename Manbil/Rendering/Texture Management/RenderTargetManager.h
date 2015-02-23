#pragma once

#include <unordered_map>
#include "../../RenderTarget.h"


//Handles creating, resizing, and deleting render targets.
//Automatically cleans up all render targets 
class RenderTargetManager
{
public:

    static const unsigned int ERROR_ID = 0;


    RenderTargetManager(void) : nextID(1) { }
    

    //Creates a new render target and returns its unique ID.
    //If there was an error, sets the given error string and returns ERROR_ID.
    unsigned int CreateRenderTarget(PixelSizes depthPixelSize, std::string& outErrorMsg);

    //Deletes the render target with the given ID.
    //Returns whether a render target with the given ID could be found.
    bool DeleteRenderTarget(unsigned int id);

    //Gets the render target with the given ID.
    //Returns 0 if there was an error finding it.
    RenderTarget* operator[](unsigned int id);


private:

    unsigned int nextID;
    std::unordered_map<unsigned int, RenderTarget> targets;
};