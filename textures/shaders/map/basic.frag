#version 330 core

in vec2 chTex; //koordinate teksture
out vec4 outCol;

uniform sampler2D uTex; //teksturna jedinica
uniform bool hide_map;

void main()
{
    vec4 paleGreen = vec4(0.7, 1.0, 0.7, 1.0);
    vec4 standardGreen = vec4(0.45,0.6, 0.4, 1.0);
    

    if (!hide_map) {
        // Sample the texture and apply a pale green tint
        vec4 texColor = texture(uTex, chTex);
        outCol = texColor * paleGreen;
    } else {
        // Display pale green without the texture
        outCol = standardGreen;
    }

}