#include "main.h"
#include "utils/log.h"

#include <unordered_map>
#include <utility>

std::unordered_map<std::string, FunctionPtr> registeredFunctions;

void registerFunction(std::string name, FunctionPtr function) {
    if (registeredFunctions.find(name) != registeredFunctions.end()) {
        LOGI("Overriding %s", name.c_str());
    }

    registeredFunctions.insert(std::make_pair(name, function));
}

FunctionPtr getFunctionAddress(std::string name) {
    if (auto it = registeredFunctions.find(name);
        it != registeredFunctions.end()) {
        LOGI("Function found named %s", name.c_str());
        return it->second;
    }

    return nullptr;
}

void init() {
    LOGI("hi");
}
