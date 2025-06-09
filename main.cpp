// Autor: Nedeljko Tesanovic
// Opis: Testiranje dubine, Uklanjanje lica, Transformacije, Prostori i Projekcije

#define _CRT_SECURE_NO_WARNINGS
 
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

//GLM biblioteke
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shape.h"
#include "ShapeHelper.h"
#include "FrameLimitter.h"
#include "TextureLoader.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);

bool visible[4] = { true, true, true, true };
bool keyPressed[4] = { false, false, false, false };
bool rotate = true;
bool spacePressed = false;

// Callback za tastaturu
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    int index = -1;
    if (key == GLFW_KEY_1) index = 0;
    else if (key == GLFW_KEY_2) index = 1;
    else if (key == GLFW_KEY_3) index = 2;
    else if (key == GLFW_KEY_4) index = 3;

    if (index != -1)
    {
        if (action == GLFW_PRESS && !keyPressed[index])
        {
            visible[index] = !visible[index];
            keyPressed[index] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keyPressed[index] = false;
        }
    }
    else if (key == GLFW_KEY_SPACE)
    {
        if (action == GLFW_PRESS && !spacePressed)
        {
            rotate = !rotate;
            spacePressed = true;
        }
        else if (action == GLFW_RELEASE)
        {
            spacePressed = false;
        }
    }
}

int main(void)
{
    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 500;
    unsigned int wHeight = 500;
    const char wTitle[] = "[Generic Title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
    unsigned int signatureShader = createShader("signature.vert", "signature.frag");

    float vertices[] =
    {
        //X    Y    Z       R    G    B    A
        0.25, 0.5, 0.75,   1.0, 0.0, 0.0, 0.0, //Crveni trougao - Prednji
       -0.25, 0.5, 0.75,   1.0, 0.0, 0.0, 0.0,
        0.0, -0.5, 0.75,   1.0, 0.0, 0.0, 0.0,

        0.25, -0.5, 0.0,   0.0, 0.0, 1.0, 0.0, //Plavi trougao - Zadnji
       -0.25, -0.5, 0.0,   0.0, 0.0, 1.0, 0.0,
        0.0,   0.5, 0.0,   0.0, 0.0, 1.0, 0.0
    };

    glm::vec3 originalCubePositions[] =
    {
        glm::vec3(-0.5f,  0.5f, 0.0f), 
        glm::vec3(-0.5f, -0.5f, 0.0f), 
        glm::vec3( 0.5f,  0.5f, 0.0f),  
        glm::vec3( 0.5f, -0.5f, 0.0f)
    };

    glm::vec3 cubePositions[] =
    {
        originalCubePositions[0],
        originalCubePositions[1],
        originalCubePositions[2],
        originalCubePositions[3]
    };

    glm::vec3 directions[4];
    for (int i = 0; i < 4; i++)
        directions[i] = glm::normalize(-originalCubePositions[i]);

    float vertices3d[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
    float signatureVertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
        -0.3f, -1.0f,  1.0f, 0.0f,
        -0.3f, -0.9f,  1.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        -0.3f, -0.9f,  1.0f, 1.0f,
        -1.0f, -0.9f,  0.0f, 1.0f
    };
    unsigned int signatureVAO, signatureVBO;
    glGenVertexArrays(1, &signatureVAO);
    glGenBuffers(1, &signatureVBO);

    glBindVertexArray(signatureVAO);
    glBindBuffer(GL_ARRAY_BUFFER, signatureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(signatureVertices), signatureVertices, GL_STATIC_DRAW);

    // Layout: position (2), texCoord (2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3d), vertices3d, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /*unsigned int stride = (3 + 4) * sizeof(float); 
    
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);*/

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    glUseProgram(signatureShader);
    unsigned signatureTexture = loadImageToTexture("signature-text.png", 0);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            UNIFORME            +++++++++++++++++++++++++++++++++++++++++++++++++

    glm::mat4 models[] = 
    {
        glm::mat4(1.0f),
        glm::mat4(1.0f),
        glm::mat4(1.0f),
        glm::mat4(1.0f)
    };

    glm::mat4 model = glm::mat4(1.0f); //Matrica transformacija - mat4(1.0f) generise jedinicnu matricu
    unsigned int modelLoc = glGetUniformLocation(unifiedShader, "uM");
    
    glm::mat4 view; //Matrica pogleda (kamere)
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // lookAt(Gdje je kamera, u sta kamera gleda, jedinicni vektor pozitivne Y ose svijeta  - ovo rotira kameru)
    unsigned int viewLoc = glGetUniformLocation(unifiedShader, "uV");
    
    
    glm::mat4 projectionP = glm::perspective(glm::radians(90.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f); //Matrica perspektivne projekcije (FOV, Aspect Ratio, prednja ravan, zadnja ravan)
    glm::mat4 projectionO = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f); //Matrica ortogonalne projekcije (Lijeva, desna, donja, gornja, prednja i zadnja ravan)
    unsigned int projectionLoc = glGetUniformLocation(unifiedShader, "uP");


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP - PETLJA ZA CRTANJE +++++++++++++++++++++++++++++++++++++++++++++++++
    glUseProgram(unifiedShader); //Slanje default vrijednosti uniformi
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //(Adresa matrice, broj matrica koje saljemo, da li treba da se transponuju, pokazivac do matrica)
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionO));
    glBindVertexArray(VAO);

    glClearColor(0.5, 0.5, 0.5, 1.0);
    glCullFace(GL_BACK);//Biranje lica koje ce se eliminisati (tek nakon sto ukljucimo Face Culling)

    for (int i = 0; i < 4; i++)
    {
        models[i] = glm::translate(models[i], cubePositions[i]);
        models[i] = glm::scale(models[i], glm::vec3(0.5f, 0.5f, 0.5f));
    }

    glm::mat4 model2 = glm::translate(model, cubePositions[1]);
    //model = glm::translate(model, cubePositions[0]);

    //model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    model2 = glm::scale(model2, glm::vec3(0.5, 0.5, 0.5));

    const double FRAME_DURATION = 1.0 / 60.0;
    double lastTime = glfwGetTime();
    float deltaTime = 0.1;

    float rotationAngle = 0.0f;
    float rotationSpeed = glm::radians(90.0f);
    float cameraSpeed = 0.5f;
    float cameraRadius = 3.0f;

    while (!glfwWindowShouldClose(window))
    {
        glUseProgram(unifiedShader);

        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        limitFrameRate(FRAME_DURATION, &lastTime);

        float camX = cameraRadius * cos(cameraSpeed * currentTime);
        float camZ = cameraRadius * sin(cameraSpeed * currentTime);
        view = glm::lookAt(
            glm::vec3(camX, 0.0f, camZ), 
            glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        if (rotate)
            rotationAngle += rotationSpeed * deltaTime * 20.0f;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        //Testiranje dubine
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            glEnable(GL_DEPTH_TEST); //Ukljucivanje testiranja Z bafera
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            glDisable(GL_DEPTH_TEST);
        }

        //Odstranjivanje lica (Prethodno smo podesili koje lice uklanjamo sa glCullFace)
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            glEnable(GL_CULL_FACE);
        }
        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        {
            glDisable(GL_CULL_FACE);
        }

        //Mijenjanje projekcija
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionP));
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionO));
        }
        //Transformisanje trouglova
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            //model = glm::translate(model, glm::vec3(-0.01, 0.0, 0.0)); //Pomjeranje (Matrica transformacije, pomjeraj po XYZ)
            model = glm::rotate(model, glm::radians(-0.05f), glm::vec3(0.0f, 1.0f, 0.0f)); //Rotiranje (Matrica transformacije, ugao rotacije u radijanima, osa rotacije)
            //model = glm::scale(model, glm::vec3(0.99, 1.0, 1.0)); //Skaliranje (Matrica transformacije, skaliranje po XYZ)
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            //model = glm::translate(model, glm::vec3(0.01, 0.0, 0.0));
            model = glm::rotate(model, glm::radians(0.05f), glm::vec3(0.0f, 1.0f, 0.0f));
            //model = glm::scale(model, glm::vec3(1.1, 1.0, 1.0));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Osvjezavamo i Z bafer i bafer boje
        
        glBindVertexArray(VAO);

        bool allCloseToCenter = true;
        for (int i = 0; i < 4; i++)
        {
            cubePositions[i] += directions[i] * deltaTime * 20.0f; // brzina pomjeranja

            //std::cout << cubePositions[0].x << std::endl;
            if (std::abs(cubePositions[0].x) > 1.0f)
                allCloseToCenter = false;

            if (!visible[i]) continue;

            glm::mat4 model = glm::translate(models[i], cubePositions[i]);

            model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        if (allCloseToCenter == false)
        {
            for (int i = 0; i < 4; ++i)
                cubePositions[i] = originalCubePositions[i];
        }

        glUseProgram(signatureShader);
        glBindVertexArray(signatureVAO);
        glBindTexture(GL_TEXTURE_2D, signatureTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++


    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(unifiedShader);

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
