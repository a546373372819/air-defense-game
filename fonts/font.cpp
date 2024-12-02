
#include "font.h"
#include "../textures/texture.h"
#include "../circles/circle.h"



unsigned int textVAO, textVBO;
std::map<char, Character> Characters;




void setupFontVAO() {

    float vertices[] = {
        // X      Y      S    T
        0.7f,  1.0f,    1.0f, 1.0f, // Top-right
        0.7f, -1.0f,    1.0f, 0.0f, // Bottom-right
       -1.0f, -1.0f,    0.0f, 0.0f, // Bottom-left
       -1.0f,  1.0f,    0.0f, 1.0f  // Top-left
    };
    
    unsigned int stride = (2 + 2) * sizeof(float);

    unsigned int indices[] = {
    0, 1, 3, // First triangle (Top-right, Bottom-right, Top-left)
    1, 2, 3  // Second triangle (Bottom-right, Bottom-left, Top-left)
    };

    glGenVertexArrays(1, &textVAO);
    glBindVertexArray(textVAO);
    glGenBuffers(1, &textVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);




}

void loadFonts(FT_Face face, FT_Library ft) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout <<  "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));

    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void RenderText(unsigned int shader, std::string text, GLfloat x, GLfloat y, GLfloat scale=0.0008) {


    std::string::const_iterator c;
    float last_pos = x;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float w = ch.size.x * scale;  
        float h = ch.size.y * scale;  

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        x += (ch.advance >> 6) * scale;


        

        float vertices[] = {
             xpos,     ypos + h,   0.0f, 0.0f ,
             xpos,     ypos,       0.0f, 1.0f ,
             xpos + w, ypos,       1.0f, 1.0f ,
             xpos + w, ypos + h,   1.0f, 0.0f
        };


        glUseProgram(shader);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        unsigned uTexLoc = glGetUniformLocation(shader, "fTex");
        glUniform1i(uTexLoc, 0); 

        glBindVertexArray(textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        

        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D,ch.textureID);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

    }
}

void RenderAllText(unsigned int shader) {

   
    if (!friendlyBase.marker) {

        RenderText(shader, "Baza: Neostecena", 0.71f, 0.85f);
        RenderText(shader, "Rakete: 10/10", 0.74f, 0.7f);
        RenderText(shader, "Helikopteri: 0/5", 0.72f, 0.55f);
        RenderText(shader, "Oboreni: 0/5", 0.72f, 0.40f);

    }
    else {

        RenderText(shader, "Baza: "+baseHealth[friendlyBase.lives], 0.71f, 0.85f);
        RenderText(shader, "Rakete: "+std::to_string(friendlyPVO.Missiles)+"/10", 0.74f, 0.7f);
        RenderText(shader, "Helikopteri: " + std::to_string(enemyChoppers.size()) + "/5", 0.72f, 0.55f);
        RenderText(shader, "Oboreni: " + std::to_string(hits) + "/5", 0.72f, 0.40f);
        RenderChopperDistance(shader);

    }

    RenderText(shader, "SV60/2021", 0.76f, -0.85f);
    RenderText(shader, "Borislav Celar", 0.73f, -0.95f);



}

void RenderChopperDistance(unsigned int shader) {

    
    for (auto it = begin(enemyChoppers); it != end(enemyChoppers);it++) {


        Circle* chopper = *it;

        glm::vec2 direction = friendlyBase.marker->position - chopper->position;

        float distance = glm::length(direction);

        int distancee = distance * 1000.0;


        RenderText(shader, std::to_string(distancee), chopper->position.x-0.03, chopper->position.y - 0.06);


    }


}




