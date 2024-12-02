#version 330 core

in vec2 chTex; //koordinate teksture
out vec4 outCol;
uniform sampler2D fTex; //teksturna jedinica



void main()
{
    vec4 paleGreen = vec4(0.7, 1.0, 0.7, 1.0);
    vec4 standardGreen = vec4(0.45,0.6, 0.4, 1.0);
    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(fTex, chTex).r);

    outCol =vec4(0.0,0.0,0.0, 1.0) * sampled;

}