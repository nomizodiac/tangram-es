#include "marker/markerBuilder.h"
#include "marker/marker.h"
#include "scene/scene.h"
#include "style/style.h"

namespace Tangram {

MarkerBuilder::MarkerBuilder(std::shared_ptr<Scene> _scene) : m_scene(_scene) {

    m_styleContext.initFunctions(*_scene);

    // Initialize StyleBuilders
    for (auto& style : _scene->styles()) {
        m_styleBuilders[style->getName()] = style->createBuilder();
    }

}

MarkerBuilder::~MarkerBuilder() {}

void MarkerBuilder::build(Marker& _marker) {

    auto& rule = _marker.drawRule();
    auto& feature = _marker.feature();

    StyleBuilder* style = nullptr;
    {
        auto name = rule.getStyleName();
        auto it = m_styleBuilders.find(name);
        if (it != m_styleBuilders.end()) {
            style = it->second.get();
        } else {
            LOGN("Invalid style %s", name.c_str());
            return;
        }
    }

    m_styleContext.setKeywordZoom(0); // FIXME

    bool valid = m_ruleSet.evaluateRuleForContext(rule, m_styleContext);

    if (valid) {
        style->addFeature(feature, rule);
        _marker.setMesh(style->build());
    }

}

} // namespace Tangram
