#include "main.h"
#include "utils/log.h"

#include <unordered_map>
#include <utility>

std::unordered_map<std::string, GL2ESFunction> registeredGL2ESFunctions;

void registerGlFunctions(std::string name, GL2ESFunction function) {
    if (registeredGL2ESFunctions.find(name) != registeredGL2ESFunctions.end()) {
        LOGI("Overriding %s", name.c_str());
    }

    registeredGL2ESFunctions.insert(std::make_pair(name, function));
}

GL2ESFunction getGlFunctionAddress(std::string name) {
    if (auto it = registeredGL2ESFunctions.find(name);
        it != registeredGL2ESFunctions.end()) {
        LOGI("Function found named %s", name.c_str());
        return it->second;
    }

    return nullptr;
}