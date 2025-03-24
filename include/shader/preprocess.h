#pragma once

#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

namespace ShaderConverter::RegexPreprocessor {

    // This assumes the GLSL is not malformed.
    // TODO: match (vec2, vec3, etc.)
    inline void moveVariableInitialization(std::string &source) {
        std::regex uniformRegex("uniform\\s+mat4\\s+(\\w+)\\s*=\\s*([^;]+);");
        // Store each uniform's name and initializer.
        std::vector<std::pair<std::string, std::string>> uniforms;

        // First pass: process each line to extract uniform declarations
        // and replace them with declarations without initialization.
        std::istringstream iss(source);
        std::ostringstream globalStream;
        std::string line;
        while (std::getline(iss, line)) {
            std::smatch match;
            if (std::regex_search(line, match, uniformRegex)) {
                uniforms.push_back({ match[1].str(), match[2].str() });
                // Replace with just the uniform declaration.
                line = std::regex_replace(
                    line,
                    std::regex("(uniform\\s+mat4\\s+\\w+)\\s*=\\s*[^;]+;"),
                    "$1;"
                );
            }
            globalStream << line << "\n";
        }
        std::string globalModified = globalStream.str();

        // Bail if didn't matched any
        if (!uniforms.size()) return;

        // Prepare a mapping from original uniform name to the new local variable name.
        std::unordered_map<std::string, std::string> replacementMap;
        int uniformIndex = 1;
        for (const auto &p : uniforms) {
            // customMat4Var(nth index)Fix (tried to be unique here lmao
            replacementMap[p.first] = "customMat4Var" + std::to_string(uniformIndex) + "Fix";
            ++uniformIndex;
        }
        
        // Second pass: process the main function.
        std::istringstream mainStream(globalModified);
        std::ostringstream output;
        bool inMain = false;
        bool insertedLocalDecls = false;
        while (std::getline(mainStream, line)) {
            // Look for the start of main.
            if (!inMain && line.find("void main()") != std::string::npos) {
                inMain = true;
            }
            // When we find the opening brace, insert the local variable declarations.
            if (inMain && !insertedLocalDecls && line.find("{") != std::string::npos) {
                output << line << "\n";
                // Insert one declaration per uniform.
                for (const auto &p : uniforms) {
                    std::string orig = p.first;
                    std::string initializer = p.second;
                    std::string newName = replacementMap[orig];
                    output << "mat4 " << newName << " = " << initializer << "; // " << orig << "\n";
                }
                insertedLocalDecls = true;
                continue;
            }

            // For the remainder of the main function, replace all occurrences
            // of the original uniform names with the new variable names.
            if (inMain && insertedLocalDecls) {
                for (const auto &entry : replacementMap) {
                    std::regex wordRegex("\\b" + entry.first + "\\b");
                    line = std::regex_replace(
                        line, 
                        wordRegex, 
                        entry.second
                    );
                }
            }
            output << line << "\n";
        }
        source = output.str();
    }

    inline void processGLSLSource(std::string& source) {
        moveVariableInitialization(source);
    }
}