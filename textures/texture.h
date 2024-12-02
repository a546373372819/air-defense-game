#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

extern unsigned int map_VAO, map_VBO;
extern unsigned int gameover_VAO, gameover_VBO;
extern unsigned checkerTexture;
extern unsigned deathTexture;
extern unsigned victoryTexture;


unsigned loadImageToTexture(const char* filePath);
void setupMapVAO(GLuint shaderProgram);
void setupGameoverVAO(GLuint shaderProgram);
void releaseTextures();
void renderMap(GLuint shaderProgram,bool hide_map);
void renderGameOver(GLuint shaderProgram, unsigned texture);



#endif