#version 330

in float vSide;
in vec4 fragColor;

uniform float thickness;

out vec4 finalColor;

void main() {
    float halfWidth = (thickness * 0.5) + 1.0;
    float distFromCenter = abs(vSide) * halfWidth;

    // Smooth 1-pixel anti-aliased edge transition
    float alpha = 1.0 - smoothstep((thickness * 0.5) - 0.5, (thickness * 0.5) + 0.5, distFromCenter);

    if (alpha <= 0.001) discard;
    finalColor = vec4(fragColor.rgb, fragColor.a * alpha);
}
