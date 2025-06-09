#pragma once

#include <string>

namespace FFPE::Rendering::ShaderGen::Common {

inline const std::string SG_VAR_NEWLINE = "\n\n";
inline const std::string SG_STMT_NEWLINE = "\n";

inline const std::string VS_TEMPLATE = R"(#version 320 es
// FOGLTLOGLES ShaderGen : Vertex Shader

// feature inputs
{}

// default outputs
flat out lowp int vertexID;

// feature outputs
{}

void main() {{
    // feature operations
    {}

    // feature output operations
    {}
    vertexID = gl_VertexID;
}})";

inline const std::string FS_TEMPLATE = R"(#version 320 es
precision mediump float;

// FOGLTLOGLES ShaderGen : Fragment Shader

// default inputs
flat in lowp int vertexID;

// feature inputs
{}

uniform sampler2D tex0;

// feature outputs
{}

void main() {{
    // feature operations
    {}

    color *= texture(tex0, vertexTexCoord.xy);

    // feature output operations
    {}
}})";

}
