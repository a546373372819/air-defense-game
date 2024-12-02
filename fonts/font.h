#pragma once

#ifndef FONT_H
#define FONT_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>  
#include <chrono>
#include<Windows.h>
#include<mmsystem.h>
#include <glm/glm.hpp>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H


struct Character {
    GLuint textureID;  // Texture ID for the glyph
    glm::ivec2 size;   // Size of the glyph
    glm::ivec2 bearing; // Offset from baseline to the top-left of the glyph
    GLuint advance;    // Offset to advance to the next glyph
};

extern unsigned int textVAO, textVBO;
extern std::map<char, Character> Characters;

void loadFonts(FT_Face face, FT_Library ft);
void RenderText(unsigned int shader, std::string text, GLfloat x, GLfloat y, GLfloat scale);
void RenderAllText(unsigned int shader);
void RenderChopperDistance(unsigned int shader);

void setupFontVAO();


#endif
