#include "assignment.hpp"
#include <vector>
#include <utility>
#include <stdlib.h>

const float HOLD_DISTANCE = 3.0f; // how far the torch is
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

FPSCamera camera;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

/*******************************************************************************
*    Author: Jonathan Wright                                                   *
*    Date: 17/10/2020 11:12PM                                                  *
*    Purpose: OpenGL Escape Assignment                                         *

FIX WIN CUTSCENE
*******************************************************************************/
int main()
{
    const float ANTONI_MOVESPEED = 2.0f; // 2 units per frame.
    time_t seed = time(NULL);
    srand(seed);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Big Gaming", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glEnable(GL_DEPTH_TEST);
    bool done = false;
    while (!glfwWindowShouldClose(window))
    {
        int objective = 0;
        int noteCount = 0;
        bool displayingNote = false;
        bool win = false;
        bool setToInitial = false;
        camera = FPSCamera{glm::vec3{0.0f, 0.0f, 3.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, -90.0f, 0.0f, -30.0f, 30.0f};
        done = false;
        /*
            Shader Initilization
        */
        Shader shader{"shader.vs", "shader.fs"};
        Shader lampShader{"light.vs", "light.fs"};
        Shader uiShader{"ui.vs", "ui.fs"};
        // Controller object that handles all texture related things.
        cg_abstractions::TextureHandler textureController{&shader};
        float vertices[] = { // Cube.
                            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
                            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
                            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
                            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
                            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
                            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
                            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
                            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
                            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
                            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
        unsigned int VBO, cubeVAO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(cubeVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        unsigned int lightVAO;
        glGenVertexArrays(1, &lightVAO);
        glBindVertexArray(lightVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        /*
            GUI VAO & VBO.
        */
        float ui[] = {
            -1, 1, // uses quads rather than triangles via GL_TRIANGLES_STRIP.
            -1, -1,
            1, 1,
            1, -1
        };
        unsigned int uiVAO, uiVBO;
        glGenVertexArrays(1, &uiVAO);
        glGenBuffers(1, &uiVBO);
        glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ui), ui, GL_STATIC_DRAW);
        glBindVertexArray(uiVAO);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);


        try
        {
            /******************************************************************************************************************
            *   Add relevant diff (Texture)
            *   and Spec (gray block for specular)
            *******************************************************************************************************************/
            textureController.addTexture("resources/textures/bricks2.jpg", "Bricks_Diff");
            textureController.addTexture("resources/textures/bricks2_disp.jpg", "Bricks_Spec");
            textureController.addTexture("resources/textures/assignment/stonygrass.jpg", "Grass_Diff");
            textureController.addTexture("resources/textures/assignment/stonygrass_spec.jpg", "Grass_Spec");
            textureController.addTexture("resources/textures/assignment/bark-1024-colcor.png", "Bark_Diff");
            textureController.addTexture("resources/textures/assignment/bark_specular.png", "Bark_Spec");
            textureController.addTexture("resources/textures/assignment/leaves.jpg", "Leaves_Diff");
            textureController.addTexture("resources/textures/assignment/leaves_specular.jpg", "Leaves_Spec");
            textureController.addTexture("resources/textures/assignment/candle.jpg", "Candle_Diff");
            textureController.addTexture("resources/textures/assignment/candle_spec.jpg", "Candle_Spec");
            textureController.addTexture("resources/textures/assignment/fire.jpg", "Fire_Diff");
            textureController.addTexture("resources/textures/assignment/fire_spec.jpg", "Fire_Spec");
            textureController.addTexture("resources/textures/assignment/antoni face.png", "Antoni_Face_Diff");
            textureController.addTexture("resources/textures/assignment/antoni limbs.png", "Antoni_Limb_Diff");
            textureController.addTexture("resources/textures/assignment/antoni middle.png", "Antoni_Torso_Diff");
            textureController.addTexture("resources/textures/assignment/fire_spec.jpg", "Generic_Spec");
            textureController.addTexture("resources/textures/assignment/blue.jpg", "blue");
            textureController.addTexture("resources/textures/assignment/blue_specular.jpg", "blue_spec");
            textureController.addTexture("resources/textures/assignment/red_bright.jpg", "red");
            textureController.addTexture("resources/textures/assignment/red_bright_specular.jpg", "red_spec");
            textureController.addTexture("resources/textures/assignment/paper texture.jpg", "paper texture");
            textureController.addTexture("resources/textures/assignment/paper texture_specular.jpg", "paper texture specular");
            textureController.addTexture("resources/textures/assignment/container2.png", "container");
            textureController.addTexture("resources/textures/assignment/container2_specular.png", "container_specular");
            /*****************************************************************************************************************
            *   GUIS
            *****************************************************************************************************************/
            textureController.addTexture("resources/textures/assignment/Restart Prompt.png", "restart", true);
            textureController.addTexture("resources/textures/assignment/Initial Objective.png", "initialObjective", true);
            textureController.addTexture("resources/textures/assignment/final objective.png", "finalObjective", true);
            textureController.addTexture("resources/textures/assignment/Note #1.png", "note1", true);
            textureController.addTexture("resources/textures/assignment/Note 2.png", "note2", true);
            textureController.addTexture("resources/textures/assignment/Note 3.png", "note3", true);
            textureController.addTexture("resources/textures/assignment/zero note count.png", "zero notes", true);
            textureController.addTexture("resources/textures/assignment/one note count.png", "one notes", true);
            textureController.addTexture("resources/textures/assignment/two note count.png", "two notes", true);
            textureController.addTexture("resources/textures/assignment/three note count.png", "three notes", true);
            textureController.addTexture("resources/textures/assignment/win screen.png", "win screen", true);
            textureController.addTexture("resources/textures/assignment/rocket prompt prewin.png", "rocket prompt", true);
        }
        catch (cg_abstractions::TextureException &except)
        {
            std::cerr << "ERROR IN LOADING TEXTURE: " << except.what() << std::endl
                      << "Terminating Program!";
            exit(EXIT_FAILURE);
        }

        shader.use();
        textureController.bindTexture("material.diffuse", 0);
        textureController.bindTexture("material.specular", 1);

        /*
            All The Notes (Objective of Game)
        */
        glm::mat4 notes[] = {
            glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3{-1.0f, 0.0f, -1.0f}), glm::vec3{1.0f, 1.0f, 0.2f}),
            glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3{1.0f, 0.0f, -1.0f}), glm::vec3{1.0f, 1.0f, 0.2f}),
            glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3{-1.0f, 0.0f, 1.0f}), glm::vec3{1.0f, 1.0f, 0.2f})
        };
        notes[0][3].x = -80.0f; notes[0][3].z = -80.0f;
        notes[1][3].x = 80.0f; notes[1][3].z = -80.0f;
        notes[2][3].x = -80.0f; notes[2][3].z = 80.0f;

        /*
            Random Crate because why not.
        */
        glm::mat4 crate{1.0f};
        crate[3].x = 30;
        crate[3].y = -0.5f;
        crate[3].z = 30;

        /*
            Indicates towards current objective.
        */
        glm::mat4 noteIndicator = glm::scale(glm::mat4{1.0f}, glm::vec3{0.25, 0.25, 0.25});

        /*
            GameMap is a big vector list of pairs, contains each tree
            CollisionBoxes is just a vector list of hitboxes
            MAP_BOUNDARIES is the actual invisible bounds of map.
        */
        std::vector<std::pair<float, float>> collisionBoxes;
        std::vector<std::pair<glm::tmat4x4<float, glm::highp>, std::pair<std::string, std::string>>> gameMap{};
        const std::pair<float, float> MAP_BOUNDARIES[] = {
            std::make_pair(118.0f, -121.0f), std::make_pair(118.0f, 118.0f),
            std::make_pair(-121.0f, 118.0f), std::make_pair(-121.0f, -121.0f)
        };

        /*
            Candle.
        */
        cg_abstractions::Candle candle{textureController, shader, lampShader, "Candle_Diff", "Candle_Spec", "Fire_Diff", "Fire_Spec"};
        candle.translate(0.0f, -0.5f, 0.0f);
        cg_abstractions::ButtonEvent happyMode{}, projectionMode{};
        cg_abstractions::Antoni antoni{0.0f, -0.25f, -30.0f, ANTONI_MOVESPEED}; // the chad himself

        /*
            Rocket.
        */
        glm::mat4 rocketTop = glm::translate(glm::scale(glm::mat4{1.0f}, glm::vec3{0.5f, 0.5f, 0.5f}), glm::vec3{12.5f / 0.5f, 2.0f / 0.5f, -5.5f / 0.5f});
        glm::mat4 rocketBase = glm::translate(glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f, 3.0f, 1.0f}), glm::vec3{12.5f, 0.5f / 5, -5.5f});
        collisionBoxes.push_back(std::make_pair(rocketBase[3].x, rocketBase[3].z));
        collisionBoxes.push_back(std::make_pair(30, 30));

        /*
            Boost to light (Radius Increase) and DULL SHINY values
        */
        float lightBoost = 1.0f;
        const float DULL = 2.0f;
        const float SHINY = 64.0f;

        /*
            Map Generation (RNG for trees.)
        */
        for (float x = -120.0f; x < 120.0f; x += 3.0f)
        {
            for (float z = -120.0f; z < 120.0f; z += 3.0f)
            {
                gameMap.push_back(std::make_pair(
                    glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3{x, -1.0f, z}), glm::vec3{3.0f, 0.25f, 3.0f}),
                    std::make_pair("Grass_Diff", "Grass_Spec")));
                int treeRoll = rand() % 100;
                if (treeRoll < 5 && (x > camera.Position.x + 5 || x < camera.Position.x - 5) && (z > camera.Position.z + 5 || z < camera.Position.z - 5))
                {
                    bool isWithin = cg_abstractions::withinBounds(x, rocketBase[3].x, z, rocketBase[3].z);
                    for (auto note: notes) {
                        if (cg_abstractions::withinBounds(x, note[3].x, z, note[3].z)) {
                            isWithin = true;
                        }
                    }
                    if (cg_abstractions::withinBounds(x, candle.position().x, z, candle.position().z) && cg_abstractions::withinBounds(x, 30, z, 30)) {
                        isWithin = true;
                    }
                    if (isWithin) {
                        spawnTree(gameMap, x, z);
                        collisionBoxes.push_back(std::make_pair(x, z));
                    }
                }
            }
        }


        while (!done)
        {
            // per-frame time logic
            // --------------------
            shader.use();
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBindVertexArray(cubeVAO);
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            happyMode.updateDelta(deltaTime);
            projectionMode.updateDelta(deltaTime);
            // input
            // -----
            processInput(window, happyMode, projectionMode, lightBoost, done, displayingNote, win);

            /*
                End Cutscene
            */
            if (win) {
                if (setToInitial) {
                    camera.Position.x = 12.5016;
                    camera.Position.y = 0 + 0.5;
                    camera.Position.z = -0.124745;
                    camera.Yaw = -90.0f;
                    camera.Pitch = 0.0f;
                    camera.allowedYaw = false;
                    setToInitial = false;
                    camera.updateCameraVectors();
                }
                shader.setInt("attentuationActivated", 0);
                shader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
                shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
                shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
                camera.Position.y += 0.2;
                rocketBase[3].y += 0.2;
                rocketTop[3].y += 0.2;
            }

            /*
                Collision Detection
            */
            camera.lockBounds(MAP_BOUNDARIES);
            for (auto collisionBox : collisionBoxes)
            {
                camera.unitCollision(collisionBox.first, collisionBox.second);
            }
            if (!camera.torch())
            {
                camera.checkForTorch(candle.position().x, candle.position().z);
            }
            else
            {
                if (!win)
                    candle.equip(camera);
            }


            // render
            // ------
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();
            shader.setVec3("viewPos", camera.Position);

            // Most Objects in my game are wood/dirt so its DULL. (Rocket is SHINY)
            shader.setFloat("material.shininess", DULL);
            shader.setFloat("light.lightBoost", lightBoost);
            if (!camera.dead())
                shader.setVec3("light.position", glm::vec3{candle.whereGlowy().x, candle.whereGlowy().y + 0.2f, candle.whereGlowy().z});
            else
                shader.setVec3("light.position", camera.Position);

            shader.setVec3("light.ambient", 0.05f, 0.05f, 0.05f);
            if (!happyMode.getActive() && !win)
            {
                shader.setInt("attentuationActivated", 1);
                shader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
                shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
            }
            else
            {
                if (!win) {
                    if (!camera.dead())
                        shader.setInt("attentuationActivated", 0);
                    shader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
                    shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
                    shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
                }
            }

            // view/projection transformations
            glm::mat4 projection;
            if (projectionMode.getActive())
            {
                projection = glm::ortho(2.0f, -2.0f, -2.0f, 2.0f, -100.0f, 100.0f);
            }
            else
            {
                projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            }
            glm::mat4 view = camera.GetViewMatrix();
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);

            // world transformation
            glBindVertexArray(cubeVAO);
            for (auto object : gameMap)
            {
                textureController.activateTexture(object.second.first, 0);
                textureController.activateTexture(object.second.second, 1);
                shader.setMat4("model", object.first);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            /* ROCKET */
            shader.setFloat("material.shininess", SHINY);
            textureController.activateTexture("blue", 0);
            textureController.activateTexture("blue_spec", 1);
            shader.setMat4("model", rocketTop);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            textureController.activateTexture("red", 0);
            textureController.activateTexture("red_spec", 1);
            shader.setMat4("model", rocketBase);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            shader.setFloat("material.shininess", DULL);

            /* Crate */
            shader.setMat4("model", crate);
            textureController.activateTexture("container", 0);
            textureController.activateTexture("container_specular", 1);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Antoni
            antoni.render(shader, textureController, cubeVAO);
            if (!antoni.touched(camera)) {
                if (!displayingNote)
                    if (!win)
                        antoni.move(camera);
                antoni.face(camera);
            } else {
                camera.dead(true);
                camera.die();
            }


            /* Render The Paper */
            if (noteCount < 3) {
                textureController.activateTexture("paper texture", 0);
                textureController.activateTexture("paper texture specular", 1);
                float animHeight = 0.5 * sin(glfwGetTime());
                glm::mat4 selectedNote{notes[noteCount]};
                selectedNote = glm::translate(selectedNote, glm::vec3{0.0f, animHeight, 0.0f});
                shader.setMat4("model", selectedNote);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                /* Paper Indicator */
                noteIndicator = glm::scale(glm::mat4{1.0f}, glm::vec3{0.05f, 0.05f, 0.05f});
                noteIndicator[3].x = camera.Position.x; noteIndicator[3].y = camera.Position.y; noteIndicator[3].z = camera.Position.z;
                float xVector = camera.Position.x - selectedNote[3].x;
                float zVector = camera.Position.z - selectedNote[3].z;
                float h = std::sqrt(std::pow(xVector, 2) + std::pow(zVector, 2));
                xVector = xVector / h; // normalize
                zVector = zVector / h; // normalize
                textureController.activateTexture("red", 0);
                textureController.activateTexture("red_spec", 1);
                noteIndicator = glm::translate(noteIndicator, glm::vec3{ -(xVector * 5), -2.5f, -(zVector * 5)});
                noteIndicator = glm::rotate(noteIndicator, -glm::atan(zVector / xVector), glm::vec3{0.0f, 1.0f, 0.0f});
                shader.setMat4("model", noteIndicator);
                glDrawArrays(GL_TRIANGLES, 0, 36); 
                if (h <= 0.5f) {
                    noteCount++;
                    displayingNote = true;
                }
                if (noteCount == 3) {
                    objective = 1;
                }
            } else {
                if (!win) {
                    /* Indicate Towards Rocket */
                    noteIndicator = glm::scale(glm::mat4{1.0f}, glm::vec3{0.05f, 0.05f, 0.05f});
                    noteIndicator[3].x = camera.Position.x; noteIndicator[3].y = camera.Position.y; noteIndicator[3].z = camera.Position.z;
                    float xVector = camera.Position.x - rocketBase[3].x;
                    float zVector = camera.Position.z - rocketBase[3].z;
                    float h = std::sqrt(std::pow(xVector, 2) + std::pow(zVector, 2));
                    xVector = xVector / h; // normalize
                    zVector = zVector / h; // normalize
                    textureController.activateTexture("red", 0);
                    textureController.activateTexture("red_spec", 1);
                    noteIndicator = glm::translate(noteIndicator, glm::vec3{ -(xVector * 5), -2.5f, -(zVector * 5)});
                    noteIndicator = glm::rotate(noteIndicator, -glm::atan(zVector / xVector), glm::vec3{0.0f, 1.0f, 0.0f});
                    shader.setMat4("model", noteIndicator);
                    glDrawArrays(GL_TRIANGLES, 0, 36); 
                    if (h <= 2.5f) {
                        win = true;
                        setToInitial = true;
                    }
                }
            }

            glm::mat4 model;
            glBindVertexArray(cubeVAO);
            if (!camera.torch())
            {
                float animHeight = 0.5 * sin(glfwGetTime());
                candle.translate(0.0f, animHeight, 0.0f);
                candle.render(lampShader, projection, view, lightVAO);
            }
            else
            {
                if (!camera.dead())
                    candle.render(camera, lampShader, projection, view, lightVAO);
                else
                    shader.setInt("attentuationActivated", 0);
            }

            /* Render UI */
            glDisable(GL_DEPTH_TEST);
            if (camera.dead()) {
                uiShader.use();
                textureController.activateTexture("restart", 0);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
                glBindVertexArray(uiVAO);
                glm::mat4 matrix{1.0f};
                uiShader.setMat4("transMatrix", matrix);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
                glDisable(GL_BLEND);
            } else {
                uiShader.use();
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
                glBindVertexArray(uiVAO);
                glm::mat4 matrix{1.0f};
                uiShader.setMat4("transMatrix", matrix);
                textureController.activateTexture("red", 0);
                if (!win) {
                    switch (noteCount) {
                        case 0:
                            textureController.activateTexture("zero notes", 0);
                            break;
                        case 1:
                            textureController.activateTexture("one notes", 0);
                            break;
                        case 2:
                            textureController.activateTexture("two notes", 0);
                            break;
                        case 3:
                            textureController.activateTexture("three notes", 0);
                            break;
                    }
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
                    switch (objective) {
                        case 0:
                            textureController.activateTexture("initialObjective", 0);
                            break;
                        case 1:
                            textureController.activateTexture("finalObjective", 0);
                            break;
                    }
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
                    if (displayingNote) {
                        switch(noteCount) {
                            case 1:
                                textureController.activateTexture("note1", 0);
                                break;
                            case 2:
                                textureController.activateTexture("note2", 0);
                                break;
                            case 3:
                                textureController.activateTexture("note3", 0);
                                break;
                        }
                        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
                    }
                    float xVector = camera.Position.x - rocketBase[3].x;
                    float zVector = camera.Position.z - rocketBase[3].z;
                    float h = std::sqrt(std::pow(xVector, 2) + std::pow(zVector, 2));
                    if (h <= 2.5f && !win) {
                        textureController.activateTexture("rocket prompt", 0);
                        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
                    }
                } else {
                    textureController.activateTexture("win screen", 0);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
                }
                glDisable(GL_BLEND);
            }
            glEnable(GL_DEPTH_TEST);
            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &lightVAO);
        glDeleteVertexArrays(1, &uiVAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &uiVBO);
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, cg_abstractions::ButtonEvent &happyMode, cg_abstractions::ButtonEvent &projectionMode, float &light, bool &done, bool &note, bool& win)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        done = true;
    }
    if (!camera.dead() && !note && !win) {
        camera.capturePreviousPosition();
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && happyMode.getTimer() > 0.2f)
    {
        happyMode.setActive(!happyMode.getActive());
        happyMode.resetTimer();
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        light += 0.5f;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        if (light >= 0.5f)
        {
            light -= 0.5f;
        }
    }

    if (note) {
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            note = false;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        done = true;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && projectionMode.getTimer() > 0.2f)
    {
        projectionMode.setActive(!projectionMode.getActive());
        projectionMode.resetTimer();
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (!camera.dead()) {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (!camera.dead()) {
        camera.ProcessMouseScroll(yoffset);
    }
}

void spawnTree(std::vector<std::pair<glm::tmat4x4<float, glm::highp>, std::pair<std::string, std::string>>> &gameMap, float xPos, float zPos)
{
    const float GROUND_LEVEL = -1.0f;
    float treeHeight = (float)(5 + rand() / (RAND_MAX / (25 - 5 + 1) + 1));
    float leavesHeight = (float)(4 + rand() / (RAND_MAX / (8 - 4 + 1) + 1));
    glm::mat4 treeBark = glm::translate(glm::mat4{1.0f}, glm::vec3{xPos, GROUND_LEVEL, zPos}); // make the bark, translate it to ground level at the x z.
    treeBark = glm::translate(treeBark, glm::vec3{0.0f, 0.5f * treeHeight, 0.0f});             // translate it half tree height
    treeBark = glm::scale(treeBark, glm::vec3{1.0f, treeHeight, 1.0f});
    gameMap.push_back(std::make_pair(
        treeBark,
        std::make_pair("Bark_Diff", "Bark_Spec")));
    glm::mat4 leaves = glm::translate(glm::mat4{1.0f}, glm::vec3{xPos, GROUND_LEVEL, zPos});
    leaves = glm::translate(leaves, glm::vec3{0.0f, 1.0f * treeHeight, 0.0f});
    leaves = glm::scale(leaves, glm::vec3{5.0f, 1.0f * leavesHeight, 5.0f});
    gameMap.push_back(std::make_pair(
        leaves,
        std::make_pair("Leaves_Diff", "Leaves_Spec")));
}
