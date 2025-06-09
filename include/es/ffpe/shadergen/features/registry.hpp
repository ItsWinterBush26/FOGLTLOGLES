#pragma once

#include "es/ffpe/shadergen/features/base.hpp"

#include <type_traits>
#include <vector>

namespace FFPE::Rendering::ShaderGen::Feature::Registry {

namespace Data {
    inline std::vector<BaseFeature*> registeredFeatures;
}

template <typename O, typename D>
concept DerivedFrom = std::is_base_of<D, O>::value;

template<DerivedFrom<BaseFeature> F>
inline void registerFeature() {
    Data::registeredFeatures.push_back(new F());
}

}
