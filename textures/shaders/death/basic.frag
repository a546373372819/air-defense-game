#version 330 core

in vec2 chTex; //koordinate teksture
out vec4 outCol;

uniform sampler2D gameoverTex; //teksturna jedinica

void main()
{

    vec4 opacity = vec4(1.0,1.0, 1.0, 0.9);

    vec4 texColor = texture(gameoverTex, chTex);

    outCol = texColor*opacity;

}