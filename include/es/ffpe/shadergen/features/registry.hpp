#pragma once

#include "es/ffpe/shadergen/features/base.hpp"
#include "utils/log.hpp"

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
    LOGI("Registering feature : %s", typeid(F).name());
    Data::registeredFeatures.push_back(new F());
}

}
