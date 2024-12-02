#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include <glm/glm.hpp>

unsigned int map_VAO, map_VBO;
unsigned int gameover_VAO, gameover_VBO;

unsigned checkerTexture;
unsigned deathTexture;
unsigned victoryTexture;




unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

void setupMapVAO(GLuint shaderProgram) {


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

    glGenVertexArrays(1, &map_VAO);
    glBindVertexArray(map_VAO);
    glGenBuffers(1, &map_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, map_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

    checkerTexture = loadImageToTexture("./res/map.png"); 
    glBindTexture(GL_TEXTURE_2D, checkerTexture); 
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   
    glBindTexture(GL_TEXTURE_2D, 0);
    
    

    glUseProgram(shaderProgram);
    unsigned uTexLoc = glGetUniformLocation(shaderProgram, "uTex");
    glUniform1i(uTexLoc, 0); 
    glUseProgram(0);

}

void setupGameoverVAO(GLuint shaderProgram) {


    float vertices[] = {
        // X      Y      S    T
        1.0f,  0.25f,    1.0f, 0.6f, // Top-right
        1.0f, -0.25f,    1.0f, 0.36f, // Bottom-right
       -1.0f, -0.25f,    0.0f, 0.36f, // Bottom-left
       -1.0f,  0.25f,    0.0f, 0.6f  // Top-left
    };
   
    unsigned int stride = (2 + 2) * sizeof(float);

    unsigned int indices[] = {
    0, 1, 3, // First triangle (Top-right, Bottom-right, Top-left)
    1, 2, 3  // Second triangle (Bottom-right, Bottom-left, Top-left)
    };

    glGenVertexArrays(1, &gameover_VAO);
    glBindVertexArray(gameover_VAO);
    glGenBuffers(1, &gameover_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, gameover_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

    deathTexture = loadImageToTexture("./res/death.png"); 
    victoryTexture = loadImageToTexture("./res/victory.png"); 





}

void renderMap(GLuint shaderProgram, bool hide_map) {
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "hide_map"), hide_map);
    glBindVertexArray(map_VAO);

    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, checkerTexture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void renderGameOver(GLuint shaderProgram, unsigned texture) {


    unsigned gameoverTexture = texture;
    glBindTexture(GL_TEXTURE_2D, gameoverTexture); 
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   
    glBindTexture(GL_TEXTURE_2D, 0);

    

    glUseProgram(shaderProgram);
    unsigned uTexLoc = glGetUniformLocation(shaderProgram, "gameoverTex");
    glUniform1i(uTexLoc, 0); 

    glBindVertexArray(gameover_VAO);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, gameoverTexture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}




void releaseTextures() {

    glDeleteTextures(1, &checkerTexture);
    glDeleteBuffers(1, &map_VBO);
    glDeleteVertexArrays(1, &map_VAO);
}
