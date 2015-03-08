#include "BlendMode.h"

#include "OpenGLIncludes.h"
#include <assert.h>



BlendMode BlendMode::CurrentMode = BlendMode::GetOpaque();

static_assert(std::is_same<unsigned int, GLenum>::value, "GLenum is not an unsigned int now??");

unsigned int BlendMode::ToGLEnum(BlendingExpressions expr)
{
    switch (expr)
    {
        case BE_ZERO:               return GL_ZERO;
        case BE_ONE:                return GL_ONE;
        case BE_SRC_COLOR:          return GL_SRC_COLOR;
        case BE_DEST_COLOR:         return GL_DST_COLOR;
        case BE_SRC_ALPHA:          return GL_SRC_ALPHA;
        case BE_DEST_ALPHA:         return GL_DST_ALPHA;
        case BE_INVERSE_SRC_COLOR:  return GL_ONE_MINUS_SRC_COLOR;
        case BE_INVERSE_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
        case BE_INVERSE_SRC_ALPHA:  return GL_ONE_MINUS_SRC_ALPHA;
        case BE_INVERSE_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
        default:
            assert(false);
            return GL_INVALID_ENUM;
    }
}
unsigned int BlendMode::ToGLEnum(BlendingOps op)
{
    switch (op)
    {
        case BO_ADD:              return GL_FUNC_ADD;
        case BO_SUBTRACT_DEST:    return GL_FUNC_SUBTRACT;
        case BO_SUBTRACT_SRC:     return GL_FUNC_REVERSE_SUBTRACT;
        case BO_MIN:              return GL_MIN;
        case BO_MAX:              return GL_MAX;
        default:
            assert(false);
            return GL_INVALID_ENUM;
    }
}
void BlendMode::EnableMode(int bufIndex) const
{
    //If this mode is already enabled, don't mess with OpenGL state.
    if (operator==(CurrentMode))
    {
        return;
    }

    if (bufIndex < 0)
    {
        glEnable(GL_BLEND);
        glBlendFunc(ToGLEnum(SourceBlend), ToGLEnum(DestBlend));
        glBlendEquation(ToGLEnum(Op));
    }
    else
    {
        if (IsOpaque())
        {
            glDisable(GL_BLEND);
        }
        else
        {
            glEnable(GL_BLEND);
            glBlendFunci((unsigned int)bufIndex, ToGLEnum(SourceBlend), ToGLEnum(DestBlend));
            glBlendEquationi((unsigned int)bufIndex, ToGLEnum(Op));
        }
    }

    CurrentMode = *this;
}
bool BlendMode::IsOpaque(void) const
{
    return SourceBlend == BE_ONE && DestBlend == BE_ZERO &&
           (Op == BO_ADD || Op == BO_SUBTRACT_DEST || Op == BO_MAX);
}

BlendMode& BlendMode::operator=(const BlendMode& cpy)
{
    memcpy(this, &cpy, sizeof(BlendMode));
    return *this;
}
bool BlendMode::operator==(const BlendMode& other) const
{
    return memcmp(this, &other, sizeof(BlendMode)) == 0;
}