#version 330

// Repurposed standard rlgl attributes:
in vec3 vertexPosition;   // xy = current point P_i (camera-relative)
in vec3 vertexNormal;     // xy = previous point P_{i-1}, z = side (-1.0 or +1.0)
in vec2 vertexTexCoord;   // xy = next point P_{i+1}

uniform mat4 mvp;
uniform vec2 resolution;  // Screen width & height in pixels
uniform float thickness;  // Desired line width in screen pixels

out float vSide;

vec2 toScreen(vec2 worldPos) {
    vec4 clip = mvp * vec4(worldPos, 0.0, 1.0);
    return ((clip.xy / clip.w) * 0.5 + 0.5) * resolution;
}

void main() {
    vec2 pCurr = vertexPosition.xy;
    vec2 pPrev = vertexNormal.xy;
    vec2 pNext = vertexTexCoord.xy;
    float side = vertexNormal.z;
    vSide = side;

    //  Project current, previous, and next points into screen pixels
    vec4 clipCurr = mvp * vec4(pCurr, 0.0, 1.0);
    vec2 sCurr = ((clipCurr.xy / clipCurr.w) * 0.5 + 0.5) * resolution;
    vec2 sPrev = toScreen(pPrev);
    vec2 sNext = toScreen(pNext);

    // Compute incoming and outgoing screen-space unit tangents
    vec2 dirA = sCurr - sPrev;
    vec2 dirB = sNext - sCurr;

    float lenA = length(dirA);
    float lenB = length(dirB);

    // Handle endpoints where pPrev == pCurr or pNext == pCurr
    vec2 tanA = (lenA > 0.0001) ? (dirA / lenA) : normalize(dirB);
    vec2 tanB = (lenB > 0.0001) ? (dirB / lenB) : tanA;

    // Compute miter normal (bisector of normals) and miter length
    vec2 normA = vec2(-tanA.y, tanA.x);
    vec2 normB = vec2(-tanB.y, tanB.x);
    vec2 miter = normA + normB;
    float miterLen = length(miter);

    vec2 normal = (miterLen > 0.0001) ? (miter / miterLen) : normB;

    // Project onto normB to keep ribbon thickness constant across bends
    // Clamp miter scale to prevent spikes on extreme hairpin turns
    float scale = 1.0 / max(dot(normal, normB), 0.25);
    scale = min(scale, 3.0);

    // Offset in screen pixels (add 1.0px padding for anti-aliasing feather)
    float halfWidth = (thickness * 0.5) + 1.0;
    vec2 pixelOffset = normal * side * halfWidth * scale;

    // Convert screen pixel offset back to Clip Space
    vec2 ndcOffset = (pixelOffset / resolution) * 2.0;
    clipCurr.xy += ndcOffset * clipCurr.w;

    gl_Position = clipCurr;
}
