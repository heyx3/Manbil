#include "UniformCollections.h"



std::string SubroutineDefinition::GetDefinition(void) const
{
    std::string ret = "subroutine ";
    
    ret += VectorF(ReturnValueSize).GetGLSLType() + " " + Name + "(";
    for (unsigned int i = 0; i < Params.size(); ++i)
    {
        if (i > 0) ret += ", ";
        ret += Params[i].GetDeclaration();
    }
    ret += ");";

    return ret;
}


void UniformDictionary::AddUniforms(const UniformDictionary & other, bool overwrite)
{
#define UD_RUN_ITERATE(dictSrc, dictDest, ovwrdup) \
    for (auto iter = dictSrc.begin(); iter != dictSrc.end(); ++iter) \
        if (ovwrdup || dictDest.find(iter->first) == dictDest.end()) \
            dictDest[iter->first] = iter->second;

    UD_RUN_ITERATE(other.FloatUniforms, FloatUniforms, overwrite);
    UD_RUN_ITERATE(other.FloatArrayUniforms, FloatArrayUniforms, overwrite);
    UD_RUN_ITERATE(other.IntUniforms, IntUniforms, overwrite);
    UD_RUN_ITERATE(other.IntArrayUniforms, IntArrayUniforms, overwrite);
    UD_RUN_ITERATE(other.MatrixUniforms, MatrixUniforms, overwrite);
    UD_RUN_ITERATE(other.Texture2DUniforms, Texture2DUniforms, overwrite);
    UD_RUN_ITERATE(other.SubroutineUniforms, SubroutineUniforms, overwrite);
}
void UniformDictionary::ClearUniforms(void)
{
    FloatUniforms.clear();
    FloatArrayUniforms.clear();
    IntUniforms.clear();
    IntArrayUniforms.clear();
    MatrixUniforms.clear();
    Texture2DUniforms.clear();
    SubroutineUniforms.clear();
}

unsigned int UniformDictionary::GetNumbUniforms(void) const
{
    return FloatUniforms.size() + FloatArrayUniforms.size() + IntUniforms.size() + IntArrayUniforms.size() +
           MatrixUniforms.size() + Texture2DUniforms.size() + SubroutineUniforms.size();
}