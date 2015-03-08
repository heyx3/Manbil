#pragma once

#include <string>


//Different ways of adding a surface's color/alpha data onto the rendered world.
struct BlendMode
{
public:

    //The various possible contributions from the source or destination color.
    enum BlendingExpressions
    {
        BE_ZERO,
        BE_ONE,

        //Fragment color.
        BE_SRC_COLOR,
        //Back buffer color.
        BE_DEST_COLOR,

        //One minus fragment color.
        BE_INVERSE_SRC_COLOR,
        //One minus back buffer color.
        BE_INVERSE_DEST_COLOR,

        //Fragment alpha.
        BE_SRC_ALPHA,
        //Back buffer alpha.
        BE_DEST_ALPHA,

        //One minus fragment alpha.
        BE_INVERSE_SRC_ALPHA,
        //One minus back buffer alpha.
        BE_INVERSE_DEST_ALPHA,
    };
    //The various possible ways to combine each color's contribution.
    enum BlendingOps
    {
        //Add the contributions together.
        BO_ADD,
        //Subtract the destination contribution from the source contribution.
        BO_SUBTRACT_DEST,
        //Subtract the source contribution from the destination contribution.
        BO_SUBTRACT_SRC,
        //Get the smallest of the two contributions.
        BO_MIN,
        //Get the largest of the two contributions.
        BO_MAX,
    };


    //Gets the currently-active blending mode.
    static BlendMode GetCurrentMode(void) { return CurrentMode; }


    static BlendMode GetOpaque(void) { return BlendMode(BE_ONE, BE_ZERO, BO_ADD); }
    static BlendMode GetTransparent(void) { return BlendMode(BE_SRC_ALPHA, BE_INVERSE_SRC_ALPHA, BO_ADD); }
    static BlendMode GetAdditive(void) { return BlendMode(BE_ONE, BE_ONE, BO_ADD); }
    static BlendMode GetMultiply(void) { return BlendMode(BE_DEST_COLOR, BE_ZERO, BO_ADD); }


    BlendingExpressions SourceBlend, DestBlend;
    BlendingOps Op;


    BlendMode(BlendingExpressions sourceBlend, BlendingExpressions destBlend, BlendingOps op)
        : SourceBlend(sourceBlend), DestBlend(destBlend), Op(op) { }
    BlendMode(const BlendMode& cpy) { *this = cpy; }


    BlendMode& operator=(const BlendMode& cpy);

    bool operator==(const BlendMode& other) const;
    bool operator!=(const BlendMode& other) const { return !(other == *this); }


    //Sets OpenGL to start using this rendering mode for both color and alpha blending.
    //If "bufferIndex" is a non-negative value,
    //    the blend mode will only be enabled for the given color attachment
    //    on the current render target.
    void EnableMode(int bufferIndex = -1) const;

    //Gets whether this mode specifies opaque rendering.
    bool IsOpaque(void) const;


private:

    static BlendMode CurrentMode;


    static unsigned int ToGLEnum(BlendingExpressions expr),
                        ToGLEnum(BlendingOps op);
};