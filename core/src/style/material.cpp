#include "material.h"

#include "platform.h"
#include "gl/shaderProgram.h"
#include "gl/texture.h"
#include "gl/renderState.h"

#include "shaders/material_glsl.h"

namespace Tangram {

Material::Material() {
}

void Material::setEmission(glm::vec4 _emission){
    m_emission = _emission;
    m_emission_texture.tex.reset();
    setEmissionEnabled(true);
}

void Material::setEmission(MaterialTexture _emissionTexture){
    m_emission_texture = _emissionTexture;
    m_emission = glm::vec4(m_emission_texture.amount, 1.f);
    setEmissionEnabled((bool)m_emission_texture.tex);
}

void Material::setAmbient(glm::vec4 _ambient){
    m_ambient = _ambient;
    m_ambient_texture.tex.reset();
    setAmbientEnabled(true);
}

void Material::setAmbient(MaterialTexture _ambientTexture){
    m_ambient_texture = _ambientTexture;
    m_ambient = glm::vec4(m_ambient_texture.amount, 1.f);
    setAmbientEnabled((bool)m_ambient_texture.tex);
}

void Material::setDiffuse(glm::vec4 _diffuse){
    m_diffuse = _diffuse;
    m_diffuse_texture.tex.reset();
    setDiffuseEnabled(true);
}

void Material::setDiffuse(MaterialTexture _diffuseTexture){
    m_diffuse_texture = _diffuseTexture;
    m_diffuse = glm::vec4(m_diffuse_texture.amount, 1.f);
    setDiffuseEnabled((bool)m_diffuse_texture.tex);
}

void Material::setSpecular(glm::vec4 _specular){
    m_specular = _specular;
    m_specular_texture.tex.reset();
    setSpecularEnabled(true);
}

void Material::setSpecular(MaterialTexture _specularTexture){
    m_specular_texture = _specularTexture;
    m_specular = glm::vec4(m_specular_texture.amount, 1.f);
    setSpecularEnabled((bool)m_specular_texture.tex);
}

void Material::setShininess(float _shiny) {
    m_shininess = _shiny;
    setSpecularEnabled(true);
}

void Material::setEmissionEnabled(bool _enable) { m_bEmission = _enable; }
void Material::setAmbientEnabled(bool _enable) { m_bAmbient = _enable; }
void Material::setDiffuseEnabled(bool _enable) { m_bDiffuse = _enable; }
void Material::setSpecularEnabled(bool _enable) { m_bSpecular = _enable; }

void Material::setNormal(MaterialTexture _normalTexture){
    m_normal_texture = _normalTexture;
    if (m_normal_texture.mapping == MappingType::spheremap){
        m_normal_texture.mapping = MappingType::planar;
    }
}

std::string mappingTypeToString(MappingType type) {
    switch(type) {
        case MappingType::uv:        return "UV";
        case MappingType::planar:    return "PLANAR";
        case MappingType::triplanar: return "TRIPLANAR";
        case MappingType::spheremap: return "SPHEREMAP";
        default:                     return "";
    }
}

std::string Material::getDefinesBlock(){
    std::string defines = "";

    bool mappings[4] = { false };

    if (m_bEmission) {
        defines += "#define TANGRAM_MATERIAL_EMISSION\n";
        if (m_emission_texture.tex) {
            defines += "#define TANGRAM_MATERIAL_EMISSION_TEXTURE\n";
            defines += "#define TANGRAM_MATERIAL_EMISSION_TEXTURE_" +
                mappingTypeToString(m_emission_texture.mapping) + "\n";
            mappings[(int)m_emission_texture.mapping] = true;
        }
    }

    if (m_bAmbient) {
        defines += "#define TANGRAM_MATERIAL_AMBIENT\n";
        if (m_ambient_texture.tex) {
            defines += "#define TANGRAM_MATERIAL_AMBIENT_TEXTURE\n";
            defines += "#define TANGRAM_MATERIAL_AMBIENT_TEXTURE_" +
                mappingTypeToString(m_ambient_texture.mapping) + "\n";
            mappings[(int)m_ambient_texture.mapping] = true;
        }
    }

    if (m_bDiffuse) {
        defines += "#define TANGRAM_MATERIAL_DIFFUSE\n";
        if (m_diffuse_texture.tex) {
            defines += "#define TANGRAM_MATERIAL_DIFFUSE_TEXTURE\n";
            defines += "#define TANGRAM_MATERIAL_DIFFUSE_TEXTURE_" +
                mappingTypeToString(m_diffuse_texture.mapping) + "\n";
            mappings[(int)m_diffuse_texture.mapping] = true;
        }
    }

    if (m_bSpecular) {
        defines += "#define TANGRAM_MATERIAL_SPECULAR\n";
        if (m_specular_texture.tex) {
            defines += "#define TANGRAM_MATERIAL_SPECULAR_TEXTURE\n";
            defines += "#define TANGRAM_MATERIAL_SPECULAR_TEXTURE_" +
                mappingTypeToString(m_specular_texture.mapping) + "\n";
            mappings[(int)m_specular_texture.mapping] = true;
        }
    }

    if (m_normal_texture.tex){
        defines += "#define TANGRAM_MATERIAL_NORMAL_TEXTURE\n";
        defines += "#define TANGRAM_MATERIAL_NORMAL_TEXTURE_" +
            mappingTypeToString(m_normal_texture.mapping) + "\n";
        mappings[(int)m_specular_texture.mapping] = true;
    }

    for (int i = 0; i < 4; i++) {
        if (mappings[i]) {
            defines += "#define TANGRAM_MATERIAL_TEXTURE_" + mappingTypeToString((MappingType)i) + "\n";
        }
    }

    return defines;
}

std::string Material::getClassBlock() {
    return SHADER_SOURCE(material_glsl);
}

std::unique_ptr<MaterialUniforms> Material::injectOnProgram(ShaderProgram& _shader ) {
    _shader.addSourceBlock("defines", getDefinesBlock(), false);
    _shader.addSourceBlock("material", getClassBlock(), false);
    _shader.addSourceBlock("setup", "material = u_material;", false);

    if (m_bEmission || m_bAmbient || m_bDiffuse || m_bSpecular || m_normal_texture.tex) {
        return std::make_unique<MaterialUniforms>(_shader);
    }
    return nullptr;
}

void Material::setupProgram(MaterialUniforms& _uniforms) {

    auto& u = _uniforms;

    if (m_bEmission) {
        u.shader.setUniformf(u.emission, m_emission);

        if (m_emission_texture.tex) {
            m_emission_texture.tex->update(RenderState::nextAvailableTextureUnit());
            m_emission_texture.tex->bind(RenderState::currentTextureUnit());
            u.shader.setUniformi(u.emissionTexture, RenderState::currentTextureUnit());
            u.shader.setUniformf(u.emissionScale, m_emission_texture.scale);
        }
    }

    if (m_bAmbient) {
        u.shader.setUniformf(u.ambient, m_ambient);

        if (m_ambient_texture.tex) {
            m_ambient_texture.tex->update(RenderState::nextAvailableTextureUnit());
            m_ambient_texture.tex->bind(RenderState::currentTextureUnit());
            u.shader.setUniformi(u.ambientTexture, RenderState::currentTextureUnit());
            u.shader.setUniformf(u.ambientScale, m_ambient_texture.scale);
        }
    }

    if (m_bDiffuse) {
        u.shader.setUniformf(u.diffuse, m_diffuse);

        if (m_diffuse_texture.tex) {
            m_diffuse_texture.tex->update(RenderState::nextAvailableTextureUnit());
            m_diffuse_texture.tex->bind(RenderState::currentTextureUnit());
            u.shader.setUniformi(u.diffuseTexture, RenderState::currentTextureUnit());
            u.shader.setUniformf(u.diffuseScale, m_diffuse_texture.scale);
        }
    }

    if (m_bSpecular) {
        u.shader.setUniformf(u.specular, m_specular);
        u.shader.setUniformf(u.shininess, m_shininess);

        if (m_specular_texture.tex) {
            m_specular_texture.tex->update(RenderState::nextAvailableTextureUnit());
            m_specular_texture.tex->bind(RenderState::currentTextureUnit());
            u.shader.setUniformi(u.specularTexture, RenderState::currentTextureUnit());
            u.shader.setUniformf(u.specularScale, m_specular_texture.scale);
        }
    }

    if (m_normal_texture.tex) {
        m_normal_texture.tex->update(RenderState::nextAvailableTextureUnit());
        m_normal_texture.tex->bind(RenderState::currentTextureUnit());
        u.shader.setUniformi(u.normalTexture, RenderState::currentTextureUnit());
        u.shader.setUniformf(u.normalScale, m_normal_texture.scale);
        u.shader.setUniformf(u.normalAmount, m_normal_texture.amount);
    }
}

}
