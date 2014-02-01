#include "PostProcessEffect.h"


PostProcessEffect()

const std::string & PostProcessEffect::GetErrorMessage(void) const
{
    if (errorMsg.empty())
        if (!material.HasError())
            if (!renderTarget.HasError())
                return errorMsg;
            else return renderTarget.GetErrorMessage();
        else return material.GetErrorMessage();
    else return errorMsg;
}

PostProcessEffect::~PostProcessEffect(void)
{
    glDeleteBuffers(1, &vbo);
}