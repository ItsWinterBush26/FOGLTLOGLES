#pragma once

#include "es/ffpe/shadergen/features/base.hpp"
#include "utils/log.hpp"

#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace FFPE::Rendering::ShaderGen::Feature::Registry {

namespace Data {
    inline std::unordered_map<std::type_index, std::unique_ptr<BaseFeature>> registeredFeatures;
}

template <typename O, typename D>
concept DerivedFrom = std::is_base_of<D, O>::value;

template<DerivedFrom<BaseFeature> F>
inline void registerFeature() {
    LOGI("Registering feature : %s", typeid(F).name());
    Data::registeredFeatures.insert({
        typeid(F),
        std::make_unique<F>()
    });
}

template<DerivedFrom<BaseFeature> F>
inline F* getFeatureInstance() {
    auto it = Data::registeredFeatures.find(typeid(F));
    if (it != Data::registeredFeatures.end()) {
        return static_cast<F*>(it->second);
    }
    return nullptr;
}

}
