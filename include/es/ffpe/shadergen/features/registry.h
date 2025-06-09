#pragma once

#include "es/ffpe/shadergen/features/base.h"

#include <unordered_set>

namespace FFPE::Rendering::ShaderGen::Feature::Registry {

namespace Data {
    std::unordered_set<BaseFeature> registeredFeatures;
}

template<typename F>
inline void registerFeature() {
    Data::registeredFeatures.insert(F());
}


}
