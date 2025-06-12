#pragma once

#include "es/ffpe/shadergen/features/base.hpp"
#include "utils/log.hpp"

#include <memory>
#include <type_traits>
#include <vector>

namespace FFPE::Rendering::ShaderGen::Feature::Registry {

namespace Data {
    inline std::vector<std::unique_ptr<BaseFeature>> registeredFeatures;
}

template <typename O, typename D>
concept DerivedFrom = std::is_base_of<D, O>::value;

template<DerivedFrom<BaseFeature> F>
inline void registerFeature() {
    LOGI("Registering feature : %s", typeid(F).name());
    Data::registeredFeatures.push_back(std::make_unique<F>());
}

template<DerivedFrom<BaseFeature> F>
inline F* getFeatureInstance() {
    for (const auto& ptr : Data::registeredFeatures) {
        if (F* res = dynamic_cast<F*>(ptr.get())) {
            return res;
        }
    }

    return nullptr;
}

}
