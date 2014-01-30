#include "Lighting.h"

#include <sstream>

void ShaderCodeGenerator::GetPhongLightingCode(std::string & out, char * tempVarsSuffix, PhongLightingVarInfo & info)
{
	const char *lightDataName = info.lightStructName,
			   *surfaceNormalName = info.surfaceNormal,
			   *lightDirName = info.lightDir,
			   *eyePosName = info.eyePos,
			   *fragWorldPosName = info.fragWorldPos,
			   *specularPowerName = info.specularPower,
			   *specularIntensityName = info.specularIntensity,
			   *outColName = info.outColName;

	//Temp names.
   std::string ambColS = std::string("ambientColor"),
			   diffColS = std::string("diffuseColor"),
		       specColS = std::string("specularColor"),

			   normalS = std::string("Normal"),
			   
			   diffFactorS = std::string("diffFactor"),
			   specFactorS = std::string("specFactor"),
			   
			   vertexToEyeS = std::string("vertexToEye"),
			   lightReflectS = std::string("reflectedLight"),
			   
			   lightColS = std::string(lightDataName),
			   lightAmbIntensityS = std::string(lightDataName),
			   lightDiffIntensityS = std::string(lightDataName);
   ambColS += tempVarsSuffix;
   diffColS += tempVarsSuffix;
   specColS += tempVarsSuffix;
   normalS += tempVarsSuffix;
   diffFactorS += tempVarsSuffix;
   specFactorS += tempVarsSuffix;
   vertexToEyeS += tempVarsSuffix;
   lightReflectS += tempVarsSuffix;
   lightColS += ".Col";
   lightAmbIntensityS += ".Ambient";
   lightDiffIntensityS += ".Diffuse";

   const char *ambCol = ambColS.c_str(),
			  *diffCol = diffColS.c_str(),
		      *specCol = specColS.c_str(),

			  *normal = normalS.c_str(),
			   
			  *diffFactor = diffFactorS.c_str(),
			  *specFactor = specFactorS.c_str(),
			   
			  *vertexToEye = vertexToEyeS.c_str(),
			  *lightReflect = lightReflectS.c_str(),
			   
			  *lightCol = lightColS.c_str(),
			  *lightAmbIntensity = lightAmbIntensityS.c_str(),
			  *lightDiffIntensity = lightDiffIntensityS.c_str();

	std::stringstream ss;
	//TODO: Remove the conditional.
	ss << "\tvec4 " << ambCol << " = vec4(" << lightCol << ", 1.0f) * " << lightAmbIntensity << ";\n\n" <<
		  "\tfloat " << diffFactor << " = dot(" << surfaceNormalName << ", " << lightDirName << ");\n\n" <<
		  "\tvec4 " << diffCol << " = vec4(0, 0, 0, 0);\n\tvec4 " << specCol << " = vec4(0, 0, 0, 0);\n\n" <<
		  "\tif (" << diffFactor << " > 0.0) {\n\t\t" <<
			diffCol << " = vec4(" << lightCol << ", 1.0f) * " << lightDiffIntensity << " * " << diffFactor << ";\n" <<
			"\t\tvec3 " << vertexToEye << " = normalize(" << eyePosName << " - " << fragWorldPosName << ");\n" <<
			"\t\tvec3 " << lightReflect << " = normalize(reflect(-" << lightDirName << ", " << surfaceNormalName << "));\n" <<
			"\t\tfloat " << specFactor << " = dot(" << vertexToEye << ", " << lightReflect << ");\n\t\t" <<
			specFactor << " = pow(" << specFactor << ", " << specularPowerName << ");\n\t\t" <<
			"\t\tif (" << specFactor << " > 0) {\n\t\t\t" <<
				specCol << " = vec4(" << lightCol << ", 1.0f) * " << specularIntensityName << " * " << specFactor << ";\n" <<
			"\t\t}\n" <<
		  "\t}\n\n" <<
		  "\tvec4 " << outColName << " = (" << ambCol << " + " << diffCol << " + " << specCol << ");\n";

	out.assign(ss.str().c_str());
}