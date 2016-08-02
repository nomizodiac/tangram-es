#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include <memory>
#include <string>

namespace Tangram {

class MapProjection;
class Scene;
struct DrawRule;
struct DrawRuleData;
struct Feature;
struct StyledMesh;

class Marker {

public:

    Marker(std::shared_ptr<Scene>& scene, Feature* feature, std::string styling);

    ~Marker();

    void setMesh(std::unique_ptr<StyledMesh> mesh);

    StyledMesh* mesh();

    DrawRule& drawRule();

    const Feature& feature();

    const glm::dvec2& origin();

    const glm::mat4& modelMatrix();

protected:

    std::unique_ptr<Feature> m_feature;
    std::unique_ptr<StyledMesh> m_mesh;
    std::unique_ptr<DrawRuleData> m_drawRuleData;
    std::unique_ptr<DrawRule> m_drawRule;

    MapProjection* m_mapProjection = nullptr;

    // Origin of marker geometry relative to global projection space.
    glm::dvec2 m_origin;

    // Matrix relating marker-local coordinates to global projection space coordinates;
    // Note that this matrix does not contain the relative translation from the global origin to the marker origin.
    // Distances from the global origin are too large to represent precisely in 32-bit floats, so we only apply the
    // relative translation from the view origin to the model origin immediately before drawing the marker.
    glm::mat4 m_modelMatrix;

};

} // namespace Tangram
