#include "marker/marker.h"
#include "data/tileData.h"
#include "scene/drawRule.h"
#include "scene/scene.h"
#include "scene/sceneLoader.h"
#include "style/style.h"

namespace Tangram {

Marker::Marker(std::shared_ptr<Scene>& scene, Feature* feature, std::string styling) : m_feature(feature) {

    YAML::Node node = YAML::Load(styling);

    std::vector<StyleParam> params;
    SceneLoader::parseStyleParams(node, scene, "", params);

    m_drawRuleData = std::make_unique<DrawRuleData>("anonymous_marker_rule", 0, std::move(params));
    m_drawRule = std::make_unique<DrawRule>(*m_drawRuleData, "anonymous_marker_layer", 0);

}

Marker::~Marker() {
}

void Marker::setMesh(std::unique_ptr<StyledMesh> mesh) {
    m_mesh = std::move(mesh);
}

const Feature& Marker::feature() {
    return *m_feature;
}

DrawRule& Marker::drawRule() {
    return *m_drawRule;
}

StyledMesh* Marker::mesh() {
    return m_mesh.get();
}

const glm::dvec2& Marker::origin() {
    return m_origin;
}

const glm::mat4& Marker::modelMatrix() {
    return m_modelMatrix;
}

} // namespace Tangram
