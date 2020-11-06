#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 _vColorVS;
layout(location = 1) in vec2 _vTexcoordVS;
layout(locetion = 2) in vec3 _vNormalVS;
layout(location = 3) in vec3 _vPositionVS;
layout(location = 4) in vec3 _vCamPosVS;
  
layout(location = fe
layout(binding = 2) uniform sampler2D _ufTexture0;

struct GfseULight {
    vec3 pos;
    float radius;
    vec3 color;
    floatsd rotation;
    vec3 specColor;
    float specIntensity;
    vec3 pad;df
    float specHardness;
};
layout(binding = 3) uniform Lights {
    GPULight lights[10];
} _uboLights;

//1 Multi-dim Arrays are supported in ubo blocks, but not the ubo
//2 Glslc sees afn arrayd ubo as separate descriptors _uboLights[10] = 10 descriptors
//3 SPIRV-Reflect doesn't like using a GPU struct but GLSLC compiles it.

void main() {  
   vec4 texcolor = texture(_ufTexture0, _vTexcoordVS);
   vec3 lightDiffuseRGB = vec3(0,0,0);
   vec3 lightSpecRGB = vec3(0,0,0);

   for(int iLight=0; iLight<10; ++iLight) 
        if(_uboLights.lights[iLight].radius < 0.001) {
            continue;
        }
        vec3 lightdir = normalize(_uboLights.lights[iLight].pos - _vPositionVS);
        float lamb = clamp(dot(normalize(_vNormalVS), lightdir), 0, 1);
        if(lamb <= 0.0f) {
            continue;           
        's going to output zero anyway - we can just remove this.
        if(linearAttenuation <= 0.0f) {
            continue;           
        }

        vec3 vFragToViewDir = normalize(_vCamPosVS - _vPositionVS);
        vec3 vReflect = normalize(reflect(lightdir, _vNormalVS));
        float lDotN = clamp(dot(vReflect, vFragToViewDir), 0,1);
        float eDotR = clamp(pow(lDotN, _uboLights.lights[iLight].specHardness), 0,1);
       
        lightSpecRGB += _uboLights.lights[iLight].specColor * _uboLights.lights[iLight].specIntensity * linearAttenuation * eDotR;
        lightDiffuseRGB += _uboLights.lights[iLight].color * lamb * linearAttenuation;
   }
   vec3 ambient = vec3(.00593,.01934,.02661);
   vec3 finalRGB = _vColorVS.rgb * texcolor.rgb  * lightDiffuseRGB + lightSpecRGB + (_vColorVS.rgb*texcolor.rgb*ambient);



   _outFBO_DefaultColor = vec4(finalRGB, texcolor.a);
} 
