#include "UniformCollections.h"


UniformList::Uniform UniformList::FindUniform(std::string name) const
{
    #define SearchList(list) \
        for (unsigned int i = 0; i < list.size(); ++i) \
            if (list[i].Name == name) \
                return list[i];
    
    SearchList(Floats)
    SearchList(Ints)
    SearchList(FloatArrays)
    SearchList(IntArrays)
    SearchList(Matrices)
    SearchList(Subroutines)
    SearchList(Texture2Ds)
    SearchList(Texture3Ds)
    SearchList(TextureCubemaps)

    return Uniform("", -1);
}


void UniformDictionary::AddUniforms(const UniformDictionary& other, bool overwrite)
{
#define AddUniformsPartial(dictSrc, dictDest, ovwrdup) \
    for (auto iter = dictSrc.begin(); iter != dictSrc.end(); ++iter) \
        if (ovwrdup || dictDest.find(iter->first) == dictDest.end()) \
            dictDest[iter->first] = iter->second;
#define AddUniformArraysPartial(dictSrc, dictDest, ovwrdup) \
    for (auto iter = dictSrc.begin(); iter != dictSrc.end(); ++iter) \
        if (ovwrdup || dictDest.find(iter->first) == dictDest.end()) \
            iter->second.CopyTo(dictDest[iter->first]);

    AddUniformsPartial(other.Floats, Floats, overwrite);
    AddUniformArraysPartial(other.FloatArrays, FloatArrays, overwrite);
    AddUniformsPartial(other.Ints, Ints, overwrite);
    AddUniformArraysPartial(other.IntArrays, IntArrays, overwrite);
    AddUniformsPartial(other.Matrices, Matrices, overwrite);
    AddUniformsPartial(other.Texture2Ds, Texture2Ds, overwrite);
    AddUniformsPartial(other.Texture3Ds, Texture3Ds, overwrite);
    AddUniformsPartial(other.TextureCubemaps, TextureCubemaps, overwrite);
    AddUniformsPartial(other.Subroutines, Subroutines, overwrite);
}
void UniformDictionary::ClearUniforms(void)
{
    Floats.clear();
    FloatArrays.clear();
    Ints.clear();
    IntArrays.clear();
    Matrices.clear();
    Texture2Ds.clear();
    Texture3Ds.clear();
    TextureCubemaps.clear();
    Subroutines.clear();
}

unsigned int UniformDictionary::GetNumbUniforms(void) const
{
    return Floats.size() + FloatArrays.size() + Ints.size() + IntArrays.size() + Matrices.size() +
           Texture2Ds.size() + Texture3Ds.size() + TextureCubemaps.size() + Subroutines.size();
}