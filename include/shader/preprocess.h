#pragma once

#include <regex>
#include <sstream>
#include <string>

namespace ShaderConverter::RegexPreprocessor {
    inline void moveVariableInitialization(std::string& source) {
        std::istringstream stringStream(source);
        std::string line;
        std::regex uniformPattern("uniform\\s+mat4\\s+(\\w+)\\s*=\\s*([^;]*);");
        std::vector<std::string> uniformDeclarations;
        
        // First pass: collect uniform declarations with initializations
        while (std::getline(stringStream, line)) {
            std::smatch match;
            if (std::regex_search(line, match, uniformPattern)) {
                // Collect the assignment part to be reinserted into main()
                uniformDeclarations.push_back(match[1].str() + " = " + match[2].str() + ";");
            }
        }
        
        // Remove uniform initializations from the source, leaving only the declaration
        std::regex removePattern("(uniform\\s+mat4\\s+\\w+)\\s*=\\s*[^;]*;");
        source = std::regex_replace(source, removePattern, "$1;");
    
        // Second pass: reinsert initialization assignments inside main()
        std::istringstream secondStream(source);
        std::ostringstream result;
        bool mainFound = false;
        bool insertedUniforms = false;
        
        while (std::getline(secondStream, line)) {
            // Look for "void main()" or "void main ()" to mark the start of the main function
            if (!mainFound && (line.find("void main()") != std::string::npos || line.find("void main ()") != std::string::npos)) {
                mainFound = true;
            }

            // When we find the opening brace, insert the initialization assignments immediately after it
            if (mainFound && !insertedUniforms && line.find("{") != std::string::npos) {
                result << line << "\n";

                for (const auto& decl : uniformDeclarations) {
                    result << decl << "\n";
                }

                insertedUniforms = true;
            } else {
                result << line << "\n";
            }
        }
        
        source = result.str();
    }    

    inline void processGLSLSource(std::string& source) {
        moveVariableInitialization(source);
    }
}