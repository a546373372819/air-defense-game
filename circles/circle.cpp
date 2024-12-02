#include "circle.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <sstream>
#include <chrono>
#include <GLFW/glfw3.h>
#include <thread>
#include "../fonts/font.h"




std::vector<Circle*> circles;
unsigned int circleVAO, circleVBO;
extern PVO friendlyPVO=PVO(nullptr);
extern Base friendlyBase=Base(nullptr);
Circle* targetChopper = nullptr;

std::string baseHealth[3]
= { "Unistena","Ostecena","Neostecena" };

std::vector<Missile*> activeMissiles;
std::vector<Circle*> enemyChoppers;
std::vector<Circle*> inactiveChoppers;
int createdChoppers;

int hits=0;
extern unsigned int wWidth;
extern unsigned int wHeight;



void addCircleToList(float x, float y) {

    glm::vec4 color;

    if (!friendlyPVO.marker) {
        color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); 
        friendlyPVO.marker = new Circle(glm::vec2(x, y), 0.03f, color);
        circles.push_back(friendlyPVO.marker);
    }
    else if (!friendlyBase.marker) {
        color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); 
        friendlyBase.marker = new Circle(glm::vec2(x, y), 0.03f, color);
        circles.push_back(friendlyBase.marker);
        generateEnemies();
    }

   
}

void generateEnemies() {


    createdChoppers = glfwGetTime()+3;
    float x, y;
    int side;

    for (int i = 0; i < 5; i++) {

    
        side= rand() % 4;


        switch (side)
        {
            case 0:
                x = -1 + (rand() / static_cast<float>(RAND_MAX)) * 1.7;
                y = 1;
                break;
            case 2:
                x = -1 + (rand() / static_cast<float>(RAND_MAX)) * 1.7;
                y = -1;
                break;
            case 1:
                y = (rand() / static_cast<float>(RAND_MAX)) * 2 - 1;
                x = 0.7;
                break;
            case 3:
                y = (rand() / static_cast<float>(RAND_MAX)) * 2 - 1;
                x = -1;
                break;
            default:
                break;
        }

        glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        Circle* enemy = new Circle(glm::vec2(x, y), 0.03f, color);
        inactiveChoppers.push_back(enemy);

    }


}

std::vector<glm::vec2> generateCircleVertices(float radius, int segments) {
    std::vector<glm::vec2> vertices;
    vertices.emplace_back(0.0f, 0.0f); 
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.1415926 * i / segments;
        vertices.emplace_back(radius * cos(angle), radius * sin(angle));
    }

    return vertices;
}

void setupCircleVAO() {
    auto vertices = generateCircleVertices(1.0f); 

    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderCircles(GLuint shaderProgram) {
    glUseProgram(shaderProgram);
    glBindVertexArray(circleVAO);

    for (const auto& circle : circles) {
        glUniform4f(glGetUniformLocation(shaderProgram, "aColor"), circle->color[0], circle->color[1], circle->color[2], circle->color[3]);
        glUniform2f(glGetUniformLocation(shaderProgram, "circlePos"), circle->position.x, circle->position.y);
        glUniform1f(glGetUniformLocation(shaderProgram, "scale"), circle->radius);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 66);
    }

    if (inactiveChoppers.size() != 0) ActivateChopper();
    updateChopperPosition();
    updateChopperColors();
    updateMissilePosition();



    glBindVertexArray(0);
}

void ActivateChopper() {
    if (glfwGetTime() >= createdChoppers + 1) {
        createdChoppers = glfwGetTime();
        enemyChoppers.push_back(inactiveChoppers.back());
        circles.push_back(inactiveChoppers.back());

        inactiveChoppers.pop_back();
    }
}


void updateChopperPosition() {


    for (auto it = begin(enemyChoppers); it != end(enemyChoppers);) {


         Circle* chopper = *it;


        glm::vec2 direction = friendlyBase.marker->position - chopper->position;

        float distance = glm::length(direction);

        if (distance <= 0.04f) {

            it = enemyChoppers.erase(it);
            circles.erase(std::find(circles.begin(), circles.end(), chopper));

            friendlyBase.lives -= 1;
            continue;
        }

        direction = glm::normalize(direction);

        chopper->position += direction * 0.001f; 
        ++it;

    }


}

void updateMissilePosition() {

    for (auto it = begin(activeMissiles); it != end(activeMissiles);) {


        Missile* missile = *it;
        glm::vec2 direction;



        direction = missile->target->position - missile->marker->position;


        float distance = glm::length(direction);

        if (distance <= 0.04f) {


            int random = rand()%100;

            if (random < 75) {

                auto itt = std::find(enemyChoppers.begin(), enemyChoppers.end(), missile->target);

                if (itt != enemyChoppers.end()) {
                    enemyChoppers.erase(itt);
                }

                itt = std::find(circles.begin(), circles.end(), missile->target);

                if (itt != circles.end()) {
                    circles.erase(itt);
                    hits++;
                }

               
                
           
            }

            it = activeMissiles.erase(it);
            circles.erase(std::find(circles.begin(), circles.end(), missile->marker));
            delete missile->marker;

            continue;
        }

        direction = glm::normalize(direction);

        missile->marker->position += direction * 0.002f; 
        ++it;
    }


}

void updateChopperColors() {

    float speedFactor = 0.4f;
    float time = glfwGetTime();

    for (auto* chopper : enemyChoppers) {

        if (chopper == targetChopper) continue;

        float distance = glm::length(friendlyBase.marker->position - chopper->position);

        float frequency = 1.0f + (1.0f / (distance + 0.1f)); 

        float pulse = (sin(time * frequency *speedFactor * 2.0f * 3.1415926f) + 1.0f) / 2.0f; 

        glm::vec4 baseColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        glm::vec4 white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);     
        chopper->color = baseColor * (1.0f - pulse) + white * pulse;

        
    }
}

void highlightEnemy(float x_click, float y_click){

    float radius = 0.03f;

    if(targetChopper)targetChopper->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    targetChopper = nullptr;

    for (Circle* chopper : enemyChoppers)
    {
        float center_x = chopper->position.x;
        float center_y = chopper->position.y;

        if (pow((x_click - center_x), 2) + pow((y_click - center_y), 2) < pow(radius, 2)) {
            chopper->color= glm::vec4(0.5f, 0.3f, 0.6f, 1.0f);
            targetChopper = chopper;
            return;
        }
    }

}

void shootMissile() {

    if (!targetChopper) return;

    if (friendlyPVO.Missiles == 0) return;


    glm::vec4 color = glm::vec4(1.0f, 0.55f, 0.0f, 1.0f);

    Circle* missile_marker = new Circle(glm::vec2(friendlyPVO.marker->position.x, friendlyPVO.marker->position.y), 0.03f, color);

    Missile* missile =new  Missile(missile_marker, targetChopper);
    activeMissiles.push_back(missile);

    circles.push_back(missile_marker);

    targetChopper = nullptr;
    friendlyPVO.Missiles--;
}

