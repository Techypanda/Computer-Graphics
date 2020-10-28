// Class defined in header
#ifndef ABSTRACTIONS_HPP
#define ABSTRACTIONS_HPP
#include "assignment.hpp"
#include <unordered_map>
#include <exception>
#include <string>
#include <iostream>

namespace cg_abstractions { //(x > camera.Position.x + 5 || x < camera.Position.x - 5) && (z > camera.Position.z + 5 || z < camera.Position.z - 5)
    bool withinBounds(float x, float otherX, float z, float otherZ) {
        return (x > otherX + 5 || x < otherX - 5) && (z > otherZ + 5 || z < otherZ - 5);
    }
    constexpr double pi() {
        return std::atan(1)*4;
    }
    static bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
    }
    /*
        Thrown when a texture couldnt be read.
    */
    class TextureException : virtual public std::exception {
        const char* reason;
        public:
            TextureException(const char* _reason) : reason{_reason} {}
            const char* what() const throw() {
                return reason;
            }
    };
    /*
        Button Model
        Holds data relating to button events.
    */
    class ButtonEvent {
        bool isActive;
        float timer;
        public:
            ButtonEvent() : isActive{false}, timer{0.0f} {}
            bool getActive() {
                return isActive;
            }
            void setActive(bool status) {
                isActive = status;
            }
            float getTimer() {
                return timer;
            }
            void updateDelta(float deltaTime) {
                timer += deltaTime;
            }
            void resetTimer() {
                timer = 0.0f;
            }
    };
    /*
        Handles Textures (Controller)
    */
    class TextureHandler {
        int width, height, nrChannels;
        std::unordered_map<std::string, unsigned int*> textures;
        Shader* shader;
        public:
            TextureHandler(Shader* shader) {
                this->shader = shader;
            }
            ~TextureHandler() {
                for (std::unordered_map<std::string, unsigned int*>::iterator it = textures.begin(); it != textures.begin(); ++it) {
                    delete it->second;
                }
            }
            void addTexture(std::string textureSource, std::string textureTitle) throw(TextureException) {
                unsigned int* textureLoc = new unsigned int{};
                glGenTextures(1, textureLoc);
                glBindTexture(GL_TEXTURE_2D, *textureLoc);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                stbi_set_flip_vertically_on_load(true);
                int width, height, nrChannels;
                unsigned char* data = stbi_load(FileSystem::getPath(textureSource).c_str(), &width, &height, &nrChannels, 0);
                if (data) {
                    GLenum format;
		            if (nrChannels == 1)
			            format = GL_RED;
		            else if (nrChannels == 3)
			            format = GL_RGB;
		            else if (nrChannels == 4)
			            format = GL_RGBA;
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                    glGenerateMipmap(GL_TEXTURE_2D);
                } else {
                    throw TextureException{"Unable To Load Texture"};
                }
                stbi_image_free(data);
                textures.insert({ textureTitle, textureLoc });
            }
            // for GUITextures, Dont flip STBI.
            void addTexture(std::string textureSource, std::string textureTitle, bool dFlip) throw(TextureException) {
                unsigned int* textureLoc = new unsigned int{};
                glGenTextures(1, textureLoc);
                glBindTexture(GL_TEXTURE_2D, *textureLoc);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                stbi_set_flip_vertically_on_load(false);
                int width, height, nrChannels;
                unsigned char* data = stbi_load(FileSystem::getPath(textureSource).c_str(), &width, &height, &nrChannels, 0);
                if (data) {
                    GLenum format;
		            if (nrChannels == 1)
			            format = GL_RED;
		            else if (nrChannels == 3)
			            format = GL_RGB;
		            else if (nrChannels == 4)
			            format = GL_RGBA;
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                    glGenerateMipmap(GL_TEXTURE_2D);
                } else {
                    throw TextureException{"Unable To Load Texture"};
                }
                stbi_image_free(data);
                textures.insert({ textureTitle, textureLoc });
            }
            /*
                Binds A Sampler2D (uniform specified by textureTitle) To textureUnit
            */
            void bindTexture(std::string textureTitle, unsigned int textureUnit) {
                shader->setInt(textureTitle, textureUnit);
            }
            void bindTexture(std::string textureTitle, unsigned int textureUnit, Shader& shaderr) {
                shaderr.setInt(textureTitle, textureUnit);
            }
            /*
                Activates a specified texture from storage to a specified textureUnit.
                NOTE: Will crash if you give it a invalid key.
            */
            void activateTexture(std::string key, unsigned int activatedTexture) {
                unsigned int texture = *(textures.find(key)->second);
                glActiveTexture(GL_TEXTURE0 + activatedTexture);
                glBindTexture(GL_TEXTURE_2D, texture);
            }
    };
    /*
        Candle player holds
    */
    class Candle {
        glm::mat4 base;
        glm::mat4 fire;
        glm::mat4 glowy;
        std::string textures[4];
        Shader* shader;
        TextureHandler* textureHandler;
        const float widthScale = 0.05f;
        const float heightScale = 0.15f;
        const float equipDistance = 150.0f;
        const float fireHeight = 0.1f;
        void alignLight() {
            fire[3].x = base[3].x; fire[3].y = base[3].y + fireHeight; fire[3].z = base[3].z;
            glowy[3] = fire[3];
        }
        public:
            Candle(TextureHandler& th, Shader& s, Shader& l, std::string baseTexture, std::string baseDiffuse, std::string fireTexture, std::string fireDiffuse) {
                this->shader = &s;
                this->textureHandler = &th;
                textures[0] = baseTexture; textures[1] = baseDiffuse;
                textures[2] = fireTexture; textures[3] = fireDiffuse;
                base = glm::scale(glm::mat4{1.0f}, glm::vec3{widthScale, heightScale, widthScale});
                fire = glm::scale(glm::mat4{1.0f}, glm::vec3{widthScale, widthScale, widthScale});
                alignLight();
            }
            void translate(float x, float y, float z) {
                base = glm::translate(base, glm::vec3{x * widthScale, y * heightScale, z * widthScale});
                alignLight();
            }
            /*
                Renders Candle, ASSUMES VAO BOUND
            */
            void render(Shader& lampShader, glm::mat4& projection, glm::mat4& view, unsigned int& lightVAO) {
                shader->use();
                textureHandler->activateTexture(textures[0], 0);
                textureHandler->activateTexture(textures[1], 1);
                shader->setMat4("model", base);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                lampShader.use();
                lampShader.setMat4("projection", projection);
                lampShader.setMat4("view", view);
                glBindVertexArray(lightVAO);
                glowy = glm::scale(glm::mat4{1.0f}, glm::vec3{widthScale, widthScale, widthScale});
                alignLight();
                lampShader.setMat4("model", glowy);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            void render(FPSCamera& camera, Shader& lampShader, glm::mat4& projection, glm::mat4& view, unsigned int& lightVAO) {
                shader->use();
                textureHandler->activateTexture(textures[0], 0);
                textureHandler->activateTexture(textures[1], 1);
                glm::mat4 basecpy = glm::rotate(base, glm::radians(-camera.Yaw), glm::vec3{0.0f, 1.0f, 0.0f});
                shader->setMat4("model", basecpy);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                lampShader.use();
                lampShader.setMat4("projection", projection);
                lampShader.setMat4("view", view);
                glBindVertexArray(lightVAO);
                glowy = glm::rotate(glm::scale(glm::mat4{1.0f}, glm::vec3{widthScale, widthScale, widthScale}), glm::radians(-camera.Yaw), glm::vec3{0.0f, 1.0f, 0.0f});
                alignLight();
                lampShader.setMat4("model", glowy);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            void equip(FPSCamera& camera) {
                float xTrans = equipDistance * glm::cos(glm::radians(camera.Yaw + 20.0f));
                float zTrans = equipDistance * glm::sin(glm::radians(camera.Yaw + 20.0f));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                setPosition(camera.Position);
                translate(xTrans, -0.5f, zTrans);
            }
            void setPosition(glm::vec3 pos) {
                base[3].x = pos.x; base[3].y = pos.y; base[3].z = pos.z;
                alignLight();
            }
            glm::vec3 position() {
                return glm::vec3{base[3].x, base[3].y, base[3].z};
            }
            glm::vec3 whereGlowy() {
                return glm::vec3{glowy[3].x, glowy[3].y, glowy[3].z};
            }
    };
    /*
        Antoni chases player.
    */
    class Antoni {
        glm::mat4 head, rLeg, lLeg, torso, lArm, rArm;
        float moveSpeed;
        void setXYZ(glm::mat4& model, float x, float y, float z) {
            model[3].x = x;
            model[3].y = y;
            model[3].z = z;
        }
        public:
            const float LIMBSCALE = 0.5f; // smoll limb.
            Antoni(const float MOVESPEED) : head{1.0f}, rLeg{1.0f}, lLeg{1.0f}, lArm{1.0f}, rArm{1.0f}, torso{1.0f} {
                moveSpeed = MOVESPEED;
                torso[3].y = lLeg[3].y + 1.0f;
                head[3].y = torso[3].y + 1.0f;
                head = glm::scale(head, glm::vec3{1.0f, 1.0f, 1.0f});
                torso = glm::scale(torso, glm::vec3{1.0f, 1.0f, 1.0f});
                lLeg = glm::translate(glm::scale(lLeg, glm::vec3{LIMBSCALE, 1.0f, 1.0f}), glm::vec3{-0.5f, 0.0f, 0.0f});
                rLeg = glm::translate(glm::scale(rLeg, glm::vec3{LIMBSCALE, 1.0f, 1.0f}), glm::vec3{0.5f, 0.0f, 0.0f});
                lArm = glm::translate(glm::scale(lArm, glm::vec3{LIMBSCALE, 1.0f, 1.0f}), glm::vec3{1.5f, 1.0f, 0.0f});
                rArm = glm::translate(glm::scale(rArm, glm::vec3{LIMBSCALE, 1.0f, 1.0f}), glm::vec3{-1.5f, 1.0f, 0.0f});
            }
            Antoni(float x, float y, float z, const float MOVESPEED) : head{1.0f}, rLeg{1.0f}, lLeg{1.0f}, lArm{1.0f}, rArm{1.0f}, torso{1.0f} {
                moveSpeed = MOVESPEED;
                setXYZ(head, x, y, z);
                setXYZ(rLeg, x, y, z);
                setXYZ(lLeg, x, y, z);
                setXYZ(rArm, x, y, z);
                setXYZ(lArm, x, y, z);
                setXYZ(torso, x, y, z);
                torso[3].y = lLeg[3].y + 1.0f;
                head[3].y = torso[3].y + 1.0f;
                head = glm::scale(head, glm::vec3{1.0f, 1.0f, 1.0f});
                torso = glm::scale(torso, glm::vec3{1.0f, 1.0f, 1.0f});
                lLeg = glm::translate(glm::scale(lLeg, glm::vec3{LIMBSCALE, 1.0f, 1.0f}), glm::vec3{-0.5f, 0.0f, 0.0f});
                rLeg = glm::translate(glm::scale(rLeg, glm::vec3{LIMBSCALE, 1.0f, 1.0f}), glm::vec3{0.5f, 0.0f, 0.0f});
                lArm = glm::translate(glm::scale(lArm, glm::vec3{LIMBSCALE, 1.0f, 1.0f}), glm::vec3{1.5f, 1.0f, 0.0f});
                rArm = glm::translate(glm::scale(rArm, glm::vec3{LIMBSCALE, 1.0f, 1.0f}), glm::vec3{-1.5f, 1.0f, 0.0f});
            }
            void render(Shader& shader, TextureHandler& textureManager, unsigned int& vao) {
                glBindVertexArray(vao);
                textureManager.activateTexture("Generic_Spec", 1);
                textureManager.activateTexture("Antoni_Face_Diff", 0);
                shader.setMat4("model", head);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                textureManager.activateTexture("Antoni_Torso_Diff", 0);
                shader.setMat4("model", torso);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                textureManager.activateTexture("Antoni_Limb_Diff", 0);
                shader.setMat4("model", lArm);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                shader.setMat4("model", rArm);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                shader.setMat4("model", lLeg);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                shader.setMat4("model", rLeg);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            void move(FPSCamera player, float DELTATIME) {
                float xVector = player.Position.x - head[3].x;
                float zVector = player.Position.z - head[3].z;
                float h = std::sqrt(std::pow(xVector, 2) + std::pow(zVector, 2));
                xVector = (xVector / h) * 5.0f;
                zVector = (zVector / h) * 5.0f;
                head = glm::translate(head, glm::vec3{xVector * DELTATIME, 0.0f, zVector * DELTATIME});
            }
            bool touched(FPSCamera player) {
                float xVector = player.Position.x - head[3].x;
                float zVector = player.Position.z - head[3].z;
                float h = std::sqrt(std::pow(xVector, 2) + std::pow(zVector, 2));
                return h <= 1.5f;
            }
            void face(FPSCamera player) {
                glm::mat4 previousStates[] = {head, torso, lArm, rArm, lLeg, rLeg};
                head = glm::mat4{1.0f};
                torso = glm::mat4{1.0f};
                lArm = glm::mat4{1.0f};
                lLeg = glm::mat4{1.0f};
                rArm = glm::mat4{1.0f};
                rLeg = glm::mat4{1.0f};
                setXYZ(head, previousStates[0][3].x, previousStates[0][3].y, previousStates[0][3].z);
                setXYZ(torso, previousStates[0][3].x, previousStates[0][3].y, previousStates[0][3].z);
                setXYZ(lArm, previousStates[0][3].x, previousStates[0][3].y, previousStates[0][3].z);
                setXYZ(rArm, previousStates[0][3].x, previousStates[0][3].y, previousStates[0][3].z);
                setXYZ(lLeg, previousStates[0][3].x, previousStates[0][3].y, previousStates[0][3].z);
                setXYZ(rLeg, previousStates[0][3].x, previousStates[0][3].y, previousStates[0][3].z);
                float h = sqrt(pow((head[3].x - player.Position.x), 2)+pow((head[3].z - player.Position.z),2));
                float d = (head[3].x - player.Position.x) / (head[3].z - player.Position.z);
                float angle = glm::atan(d);
                head = glm::rotate(head, angle, glm::vec3{0.0f, 1.0f, 0.0f});
                torso = glm::translate(glm::rotate(torso, angle, glm::vec3{0.0f, 1.0f, 0.0f}), glm::vec3{0.0f, -1.0f, 0.0f});
                lArm = glm::scale(glm::translate(glm::rotate(lArm, angle, glm::vec3{0.0f, 1.0f, 0.0f}), glm::vec3{-0.75f, -1.0f, 0.0f}), glm::vec3{0.5f, 1.0f, 1.0f});
                lLeg = glm::scale(glm::translate(glm::rotate(lLeg, angle, glm::vec3{0.0f, 1.0f, 0.0f}), glm::vec3{-0.25f, -2.0f, 0.0f}), glm::vec3{0.5f, 1.0f, 1.0f});
                rArm = glm::scale(glm::translate(glm::rotate(rArm, angle, glm::vec3{0.0f, 1.0f, 0.0f}), glm::vec3{0.75f, -1.0f, 0.0f}), glm::vec3{0.5f, 1.0f, 1.0f});
                rLeg = glm::scale(glm::translate(glm::rotate(rLeg, angle, glm::vec3{0.0f, 1.0f, 0.0f}), glm::vec3{0.25f, -2.0f, 0.0f}), glm::vec3{0.5f, 1.0f, 1.0f});
            }
    };
}
#endif