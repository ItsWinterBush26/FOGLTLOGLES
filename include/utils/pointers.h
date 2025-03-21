#pragma once

#include <memory>

template<typename T, typename... Args>
inline std::shared_ptr<T> MakeAggregateShared(Args&&... args) {
    return std::make_shared<T>(T{ std::forward<Args>(args)... });
}
