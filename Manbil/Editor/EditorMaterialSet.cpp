#include "EditorMaterialSet.h"

#include "../Rendering/GUI/TextRenderer.h"
#include "../Rendering/Materials/Data Nodes/Math/InterpolateNode.h"
#include "../Rendering/Materials/Data Nodes/Parameters/ParamNode.h"


EditorMaterialSet::EditorMaterialSet(TextRenderer & renderer)
    : TextRender(renderer), FontID(0),
      ButtonTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U_GREYSCALE, false),
      SliderBarTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U_GREYSCALE, false),
      SliderNubTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U_GREYSCALE, false),
      TextBoxBackgroundTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U_GREYSCALE, false),
      PanelBackgroundTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U_GREYSCALE, false),
      CheckBoxBackgroundTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U_GREYSCALE, false),
      CheckBoxCheckTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U_GREYSCALE, false),
      SelectionBoxBoxTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U_GREYSCALE, false),
      SelectionBoxBackgroundTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U_GREYSCALE, false)
{

}
EditorMaterialSet::~EditorMaterialSet(void)
{
    if (AnimatedMatGrey != 0) delete AnimatedMatGrey;
    if (StaticMatGrey != 0) delete StaticMatGrey;
    if (AnimatedMatColor != 0) delete AnimatedMatColor;
    if (StaticMatColor != 0) delete StaticMatColor;
}

std::string EditorMaterialSet::GenerateDefaultInstance(EditorMaterialSet & outSet)
{
    //Load the font.
    outSet.FontID = outSet.TextRender.CreateAFont("Content/Fonts/Inconsolata.otf", 80);
    if (outSet.FontID == FreeTypeHandler::ERROR_ID)
        return "Error loading font 'Content/Fonts/Inconsolata.otf': " + outSet.TextRender.GetError();


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
    greyData.Reset(1, 1, 0.6f);
    outSet.SliderBarTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.SliderBarTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for slider bar texture.";

    //Slider nub texture.
    greyData.Fill(0.1f);
    outSet.SliderNubTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.SliderNubTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for slider nub texture.";

    //Text box background texture.
    greyData.Fill(1.0f);
    outSet.TextBoxBackgroundTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.TextBoxBackgroundTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for text box background texture.";

    //Panel background texture.
    greyData.Fill(0.95f);
    outSet.PanelBackgroundTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.PanelBackgroundTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for panel background texture.";

    //SelectionBox background texture.
    greyData.Fill(0.8f);
    outSet.SelectionBoxBackgroundTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.SelectionBoxBackgroundTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for SelectionBox background texture.";

    //SelectionBox box texture.
    greyData.Reset(256, 64, 0.8f);
    for (unsigned int x = 0; x < greyData.GetWidth(); ++x)
    {
        greyData[Vector2u(x, 0)] = 0.1f;
        greyData[Vector2u(x, greyData.GetHeight() - 1)] = 0.1f;
    }
    outSet.SelectionBoxBoxTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U_GREYSCALE);
    if (!outSet.SelectionBoxBoxTex.SetGreyscaleData(greyData))
        return "Error occurred while setting texture data for SelectionBox box texture.";

    //Checkbox background texture.
    outSet.CheckBoxBackgroundTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U);
    std::string err;
    if (!outSet.CheckBoxBackgroundTex.SetDataFromFile("Content/Textures/CheckboxBackground.png", err))
    {
        return "Error loading 'CheckboxBackground.png' from 'Content/Textures': " + err;
    }

    //Checkbox check texture.
    outSet.CheckBoxCheckTex.Create(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), false, PixelSizes::PS_8U);
    if (!outSet.CheckBoxCheckTex.SetDataFromFile("Content/Textures/CheckboxCheck.png", err))
    {
        return "Error loading 'CheckboxCheck.png' from 'Content/Textures': " + err;
    }


    //Materials.

    outSet.AnimatedMatGreyParams.ClearUniforms();
    outSet.StaticMatGreyParams.ClearUniforms();
    outSet.AnimatedMatColParams.ClearUniforms();
    outSet.StaticMatColParams.ClearUniforms();
    outSet.AnimatedMatTextParams.ClearUniforms();
    outSet.StaticMatTextParams.ClearUniforms();

    ShaderGenerator::GeneratedMaterial genM =
        GUIMaterials::GenerateStaticQuadDrawMaterial(outSet.StaticMatGreyParams, GUIMaterials::TT_GREYSCALE);
    if (!genM.ErrorMessage.empty())
        return "Error creating static greyscale material: " + genM.ErrorMessage;
    outSet.StaticMatGrey = genM.Mat;

    genM = GUIMaterials::GenerateStaticQuadDrawMaterial(outSet.StaticMatColParams, GUIMaterials::TT_COLOR);
    if (!genM.ErrorMessage.empty())
        return "Error creating static color material: " + genM.ErrorMessage;
    outSet.StaticMatColor = genM.Mat;

    genM = GUIMaterials::GenerateStaticQuadDrawMaterial(outSet.StaticMatTextParams, GUIMaterials::TT_TEXT);
    if (!genM.ErrorMessage.empty())
        return "Error creating static text material: " + genM.ErrorMessage;
    outSet.StaticMatText = genM.Mat;

    typedef DataNode::Ptr DNP;
    DNP lerpParam(new ParamNode(1, GUIMaterials::DynamicQuadDraw_TimeLerp, "timeLerpParam"));
    DNP lerpColor(new InterpolateNode(outSet.MaxAnimateColor, outSet.MinAnimateColor,
                                      lerpParam, InterpolateNode::IT_Linear, "lerpColor"));
    genM = GUIMaterials::GenerateDynamicQuadDrawMaterial(outSet.AnimatedMatGreyParams, GUIMaterials::TT_GREYSCALE,
                                                         Vector2f(1.0f, 1.0f), lerpColor);
    if (!genM.ErrorMessage.empty())
        return "Error creating animated greyscale material: " + genM.ErrorMessage;
    outSet.AnimatedMatGrey = genM.Mat;

    genM = GUIMaterials::GenerateDynamicQuadDrawMaterial(outSet.AnimatedMatColParams, GUIMaterials::TT_COLOR,
                                                         Vector2f(1.0f, 1.0f), lerpColor);
    if (!genM.ErrorMessage.empty())
        return "Error creating animated color material: " + genM.ErrorMessage;
    outSet.AnimatedMatColor = genM.Mat;

    genM = GUIMaterials::GenerateDynamicQuadDrawMaterial(outSet.AnimatedMatTextParams, GUIMaterials::TT_TEXT,
                                                         Vector2f(1.0f, 1.0f), lerpColor);
    if (!genM.ErrorMessage.empty())
        return "Error creating animated text material: " + genM.ErrorMessage;
    outSet.AnimatedMatText = genM.Mat;


    return "";
}


Material* EditorMaterialSet::GetAnimatedMaterial(const MTexture2D * tex) const
{
    if (tex->IsColorTexture())
        return AnimatedMatColor;
    else if (tex->IsGreyscaleTexture())
        return AnimatedMatGrey;
    
    assert(false);
    return 0;
}
Material* EditorMaterialSet::GetStaticMaterial(const MTexture2D * tex) const
{
    if (tex->IsColorTexture())
        return StaticMatColor;
    else if (tex->IsGreyscaleTexture())
        return StaticMatGrey;
    
    assert(false);
    return 0;
}

const UniformDictionary & EditorMaterialSet::GetAnimatedMatParams(const MTexture2D * tex) const
{
    if (tex->IsColorTexture())
        return AnimatedMatColParams;
    else if (tex->IsGreyscaleTexture())
        return AnimatedMatGreyParams;

    assert(false);
    return AnimatedMatColParams;
}
const UniformDictionary & EditorMaterialSet::GetStaticMatParams(const MTexture2D * tex) const
{
    if (tex->IsColorTexture())
        return StaticMatColParams;
    else if (tex->IsGreyscaleTexture())
        return StaticMatGreyParams;

    assert(false);
    return StaticMatColParams;
}