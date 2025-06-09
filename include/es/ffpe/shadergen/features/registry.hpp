#pragma once

#include "es/ffpe/shadergen/features/base.hpp"

#include <unordered_set>

namespace FFPE::Rendering::ShaderGen::Feature::Registry {

namespace Data {
    inline std::unordered_set<BaseFeature*> registeredFeatures;
}

template<typename F>
inline void registerFeature() {
    Data::registeredFeatures.insert(new F());
}

}
