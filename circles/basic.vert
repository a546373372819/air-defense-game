#version 330 core

layout(location = 0) in vec2 aPos;  // Position

uniform vec2 circlePos;
uniform float scale;
uniform vec4 aColor;

out vec4 fragColor; // Pass color to fragment shader

float ratio=900.0/1500.0;

void main() {


    vec2 adjustedPos = vec2(aPos.x * ratio, aPos.y);
    gl_Position = vec4(adjustedPos * scale + circlePos, 0.0, 1.0);
    fragColor = aColor;
}
