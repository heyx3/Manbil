#include "Materials.h"

#include "Material.h"
#include "ShaderHandler.h"


const MaterialShaders Materials::UnlitTexture = MaterialShaders(
	std::string("void main()\n\
				{\n\
					gl_Position = worldTo4DScreen(in_pos);\n\
					out_tex = in_tex;\n\
				}"),


	std::string("uniform float brightness;\n\
                void main()\n\
				{\n\
					out_finalCol = vec4(brightness * texture2D(u_sampler0, out_tex).xyz, 1.0);\n\
				}"));



const MaterialShaders Materials::LitTexture = MaterialShaders(
	std::string("void main()\n\
				{\n\
					vec4 pos4 = worldTo4DScreen(in_pos);\n\
					gl_Position = pos4;\n\
					\n\
					out_pos = (u_world * vec4(in_pos, 1.0)).xyz;\n\
					out_tex = in_tex;\n\
					out_normal = normalize((u_world * vec4(in_normal, 0.0)).xyz);\n\
				}"),


	std::string("struct DirectionalLightStruct\n\
				{\n\
					vec3 Dir, Col;\n\
					float Ambient, Diffuse, Specular;\n\
					float SpecularIntensity;\n\
				};\n\
				uniform DirectionalLightStruct DirectionalLight;\n\
				\n\
				void main()\n\
				{\n\
					float brightness = getBrightness(normalize(out_normal),\n\
													 normalize(out_pos - u_cam_pos),\n\
													 DirectionalLight.Dir,\n\
													 DirectionalLight.Ambient, DirectionalLight.Diffuse, DirectionalLight.Specular,\n\
													 DirectionalLight.SpecularIntensity);\n\
					vec3 texCol = texture2D(u_sampler0, out_tex).xyz;\n\
					\n\
					out_finalCol = vec4(texCol * DirectionalLight.Col * brightness, 1.0);\n\
				}"));



const MaterialShaders Materials::BareColor = MaterialShaders(
	std::string("void main()\n\
				{\n\
					gl_Position = worldTo4DScreen(in_pos);\n\
					out_col = in_col;\n\
				}"),


	std::string("void main()\n\
				{\n\
					out_finalCol = vec4(out_col.xyz, 1.0);\n\
				}"));


void Materials::GetDefaultUniforms_LitTexture(FloatUniforms & floats, IntUniforms & ints, MatUniforms & mats)
{
    Vector3f tempV3(1.0f, 0.0f, 0.0f);

    floats["DirectionalLight.Dir"] = Mesh::UniformValue<float>(&tempV3[0], 3);

    tempV3 = Vector3f(1, 1, 1);
    floats["DirectionalLight.Col"] = Mesh::UniformValue<float>(&tempV3[0], 3);

    float zero = 0.0f, one = 1.0f, tenth = 0.1f;
    float thirtyTwo = 32.0f;
    floats["DirectionalLight.Ambient"] = Mesh::UniformValue<float>(&tenth, 1);
    floats["DirectionalLight.Diffuse"] = Mesh::UniformValue<float>(&one, 1);
    floats["DirectionalLight.Specular"] = Mesh::UniformValue<float>(&one, 1);
    floats["DirectionalLight.SpecularIntensity"] = Mesh::UniformValue<float>(&thirtyTwo, 1);
}

bool Materials::LitTexture_GetUniforms(Material & mat)
{
	return  mat.AddUniform("DirectionalLight.Dir") &&
			mat.AddUniform("DirectionalLight.Col") &&
			mat.AddUniform("DirectionalLight.Ambient") &&
			mat.AddUniform("DirectionalLight.Diffuse") &&
			mat.AddUniform("DirectionalLight.Specular") &&
			mat.AddUniform("DirectionalLight.SpecularIntensity");
}
bool Materials::LitTexture_SetUniforms(Material & mat, const LitTexture_DirectionalLight & lightDat)
{
	ShaderHandler::UseShader(mat.GetShaderProgram());

	return  mat.SetUniformF("DirectionalLight.Dir", &(lightDat.Dir[0]), 3) &&
			mat.SetUniformF("DirectionalLight.Col", &(lightDat.Col[0]), 3) &&
			mat.SetUniformF("DirectionalLight.Ambient", &lightDat.Ambient, 1) &&
			mat.SetUniformF("DirectionalLight.Diffuse", &lightDat.Diffuse, 1) &&
			mat.SetUniformF("DirectionalLight.Specular", &lightDat.Specular, 1) &&
			mat.SetUniformF("DirectionalLight.SpecularIntensity", &lightDat.SpecularIntensity, 1);
}