#include "EditorMaterialSet.h"

#include "../Rendering/GUI/TextRenderer.h"
#include "../Rendering/Materials/Data Nodes/Math/InterpolateNode.h"
#include "../Rendering/Materials/Data Nodes/Parameters/ParamNode.h"


EditorMaterialSet::EditorMaterialSet(TextRenderer & renderer)
    : TextRender(renderer), FontID(0),
      ButtonTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U_GREYSCALE, false),
      SliderBarTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U, false),
      SliderNubTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U, false),
      TextBoxBackgroundTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U, false),
      PanelBackgroundTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U, false)
{

}

std::string EditorMaterialSet::GenerateDefaultInstance(EditorMaterialSet & outSet)
{
    //Load the font.
    outSet.FontID = outSet.TextRender.CreateAFont("Content/Fonts/Candara.ttf", 30);
    if (outSet.FontID == FreeTypeHandler::ERROR_ID)
        return "Error loading font 'Content/Fonts/Candara.ttf': " + outSet.TextRender.GetError();


    //Set up the textures.

    //Button texture.
    Array2D<float> greyData(128, 128);
    greyData.FillFunc([](Vector2u loc, float * outVal)
    {
        const Vector2f midpoint(64.0f, 64.0f);
        const float radius = 85.0f;
        Vector2f locF = ToV2f(loc);

        if (midpoint.Distance(ToV2f(loc)) > radius)
            *outVal = 0.0f;
        else *outVal = 1.0f;
    });
    outSet.ButtonTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.ButtonTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for button texture.";

    //Slider bar texture.
    greyData.FillFunc([](Vector2u loc, float * outVal)
    {
        *outVal *= 0.6f;
    });
    outSet.SliderBarTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.SliderBarTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for slider bar texture.";

    //Slider nub texture.
    greyData.Reset(1, 1, 0.1f);
    outSet.SliderNubTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.SliderNubTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for slider nub texture.";

    //Text box background texture.
    greyData.Fill(1.0f);
    outSet.TextBoxBackgroundTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.TextBoxBackgroundTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for text box background texture.";

    //Panel background texture.
    outSet.PanelBackgroundTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.PanelBackgroundTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for panel background texture.";


    //Materials.

    outSet.AnimatedMatParams.ClearUniforms();
    outSet.StaticMatParams.ClearUniforms();

    ShaderGenerator::GeneratedMaterial genM =
        GUIMaterials::GenerateStaticQuadDrawMaterial(outSet.StaticMatParams, true);
    if (!genM.ErrorMessage.empty())
        return "Error creating static greyscale material: " + genM.ErrorMessage;

    typedef DataNode::Ptr DNP;
    DNP lerpParam(new ParamNode(1, GUIMaterials::DynamicQuadDraw_TimeLerp, "timeLerpParam"));
    DNP lerpColor(new InterpolateNode(outSet.MaxAnimateColor, outSet.MinAnimateColor,
                                      lerpParam, InterpolateNode::IT_Linear, "lerpColor"));
    genM = GUIMaterials::GenerateDynamicQuadDrawMaterial(outSet.AnimatedMatParams, true,
                                                         Vector2f(1.0f, 1.0f), lerpColor);
    if (!genM.ErrorMessage.empty())
        return "Error creating dynamic greyscale material: " + genM.ErrorMessage;


    return "";
}