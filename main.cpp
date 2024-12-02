

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "circles/circle.h"
#include "textures/texture.h"
#include "fonts/font.h"
#include <thread>  
#include <chrono>
#include<Windows.h>
#include<mmsystem.h>

#pragma comment(lib,"winmm.lib")




unsigned int wWidth = 1500;
unsigned int wHeight = 900;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);

int main(void)
{




    srand(static_cast<unsigned int>(time(0)));

    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    const char wTitle[] = "[Generic Title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }



    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    }

    FT_Face face;
    if (FT_New_Face(ft, "./res/font/sans.ttf", 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    loadFonts(face, ft);

    unsigned int mapShader = createShader("textures/shaders/map/basic.vert", "textures/shaders/map/basic.frag");
    unsigned int gameoverShader = createShader("textures/shaders/death/basic.vert", "textures/shaders/death/basic.frag");
    unsigned int circleShader = createShader("circles/basic.vert", "circles/basic.frag");
    unsigned int fontShader = createShader("fonts/shaders/basic.vert", "fonts/shaders/basic.frag");



    
    setupCircleVAO();
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    setupMapVAO(mapShader);
    setupGameoverVAO(gameoverShader);
    setupFontVAO();
    

    bool hide_map = false; 

    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window))
    {

        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            hide_map = true;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            hide_map = false;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            shootMissile();
        }


        renderMap(mapShader, hide_map);


        renderCircles(circleShader);

        if (friendlyBase.lives <= 0){
            renderGameOver(gameoverShader,deathTexture);
        }
        else if (friendlyBase.marker && enemyChoppers.size() == 0 && inactiveChoppers.size() == 0) {
            renderGameOver(gameoverShader,victoryTexture);
        }

        RenderAllText(fontShader);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (friendlyBase.lives <= 0) {
            PlaySound(TEXT("./res/you_died_sound.wav"), NULL, SND_FILENAME | SND_SYNC);
            break;
        }
        else if (friendlyBase.marker && enemyChoppers.size() == 0&&inactiveChoppers.size()==0) {
            PlaySound(TEXT("./res/victory.wav"), NULL, SND_FILENAME | SND_SYNC);
            break;
        }
    }


    releaseTextures();
    glDeleteProgram(mapShader);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        float x_ndc = (xpos / width) * 2.0f - 1.0f;
        float y_ndc = 1.0f - (ypos / height) * 2.0f; 

        addCircleToList(x_ndc, y_ndc);
        highlightEnemy(x_ndc, y_ndc);
    }
}
