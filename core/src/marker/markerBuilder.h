#pragma once

#include "scene/styleContext.h"
#include "scene/drawRule.h"
#include "util/fastmap.h"

namespace Tangram {

class Marker;
class StyleBuilder;
struct TileData;

class MarkerBuilder {

public:

    MarkerBuilder(std::shared_ptr<Scene> _scene);

    ~MarkerBuilder();

    void build(Marker& _marker);

private:

    std::shared_ptr<Scene> m_scene;
    fastmap<std::string, std::unique_ptr<StyleBuilder>> m_styleBuilders;
    StyleContext m_styleContext;
    DrawRuleMergeSet m_ruleSet;

};

}
