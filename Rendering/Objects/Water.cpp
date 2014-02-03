#include "Water.h"

RenderingPass Water::GetRippleWaterRenderer(void)
{
    return RenderingPass(std::string(
            "void main()\n\
             {\n\
                \n\
             }"),
            std::string("\
            uniform float bumpmapHeight;\n\
            void main()\n\
            {\n\
                finalPos = out_pos + vec3(0.0, 0.0, bumpmapHeight * texture(u_sampler0, in_tex).x);\n\
            }"));
}