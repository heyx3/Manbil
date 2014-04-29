#pragma once

#include <unordered_map>
#include "../../RenderTarget.h"


//Handles creating, resizing, and deleting render targets.
class RenderTargetManager
{
public:

    static const unsigned int ERROR_ID = 0;


    RenderTargetManager(void) : nextID(1) { }
    
    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }

    //Creates a new render target and returns its unique ID.
    //Returns ERROR_ID and sets the error message if there was an error creating the target.
    unsigned int CreateRenderTarget(const RendTargetColorTexSettings & colTex, const RendTargetDepthTexSettings & depthTex)
    {
        std::vector<RendTargetColorTexSettings> cols;
        cols.insert(cols.end(), colTex);
        return CreateRenderTarget(cols, depthTex);
    }
    unsigned int CreateRenderTarget(const std::vector<RendTargetColorTexSettings> & colTexes, const RendTargetDepthTexSettings & depthTex);

    //Deletes the render target with the given ID.
    //If it can't be found, sets the error message and returns "false".
    //Otherwise, returns "true".
    bool DeleteRenderTarget(unsigned int id);

    //Resizes the given render target.
    //The "attachment" variable is the color attachment to resize.
    //If a negative number is passed for "attachment", all color attachments are resized to the new value.
    bool ResizeTarget(unsigned int id, unsigned int newWidth, unsigned int newHeight, int attachment = -1);



    //Gets the render target with the given ID.
    //   Returns 0 if there was an error finding it.
    RenderTarget * operator[](unsigned int id);


private:

    std::string errorMsg;

    unsigned int nextID;
    std::unordered_map<unsigned int, RenderTarget*> targets;
};