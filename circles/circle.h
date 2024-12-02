#pragma once
#ifndef CIRCLE_H
#define CIRCLE_H

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <GL/glew.h>


struct Circle {
    glm::vec2 position;
    float radius;
    glm::vec4 color;

    Circle(glm::vec2 pos, float r, glm::vec4 col)
        : position(pos), radius(r), color(col) {}

    Circle(){}
};

struct PVO {
    int Missiles = 10;
    Circle* marker;
    PVO(Circle* circle)
        : marker(circle) {}

};

struct Base {
    int lives = 2;
    Circle* marker;
    Base(Circle* circle)
        : marker(circle) {}

};

struct Missile {
    Circle* target;
    Circle* marker;
    Missile(Circle* marker, Circle* target)
        : marker(marker),target(target) {}

};


extern std::string baseHealth[3];

extern std::vector<Circle*> circles;
extern PVO friendlyPVO;
extern Base friendlyBase;
extern Circle* targetChopper;
extern int hits;

extern std::vector<Missile*> activeMissiles;
extern std::vector<Circle*> enemyChoppers;
extern std::vector<Circle*> inactiveChoppers;


extern unsigned int circleVAO, circleVBO;

void addCircleToList(float x, float y);
void generateEnemies();
void setupCircleVAO();
void renderCircles(GLuint shaderProgram);
void updateChopperPosition();
void updateMissilePosition();
void highlightEnemy(float x_click, float y_click);
void updateChopperColors();
void shootMissile();
void ActivateChopper();



std::vector<glm::vec2> generateCircleVertices(float radius, int segments = 64);

#endif