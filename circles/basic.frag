#version 330 core

in vec4 fragColor;  // The color passed from the vertex shader
out vec4 color;

void main()
{
    color = fragColor; // Set the output color to the passed color
}
