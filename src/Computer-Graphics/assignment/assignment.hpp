#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <learnopengl/filesystem.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <iostream>
#include "FPSCamera.hpp"
#include "abstractions.hpp"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, cg_abstractions::ButtonEvent &happyMode, cg_abstractions::ButtonEvent &projectionMode, float &light, bool &done, bool &note, bool& win);
void spawnTree(std::vector<std::pair<glm::tmat4x4<float, glm::highp>, std::pair<std::string, std::string>>>& gameMap, float xPos, float zPos);
#endif