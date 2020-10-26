#ifndef FPSCAMERA_H
#define FPSCAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  10.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class FPSCamera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float PitchClamp[2]; // clamped vision
    float Yaw;
    float Pitch;
    float Roll;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    bool isDead;
    bool allowedYaw;

    // Constructor with vectors
    FPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float minPitch = -89.99f, float maxPitch = 89.99f) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        allowedYaw = true;
        Roll = 0.0f;
        isDead = false;
        hasTorch = false;
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        PitchClamp[0] = minPitch; 
        PitchClamp[1] = maxPitch;
        updateCameraVectors();
    }
    // Constructor with scalar values
    FPSCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float minPitch = -89.99f, float maxPitch = 89.99f) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        allowedYaw = true;
        Roll = 0.0f;
        isDead = false;
        hasTorch = false;
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        PitchClamp[0] = minPitch; 
        PitchClamp[1] = maxPitch;
        updateCameraVectors();
    }
    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void capturePreviousPosition() { // hacky solution...
        previousPosition = Position;
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        Position.y = 0;
    }
    bool dead() {
        return isDead;
    }
    void die() {
        if (Roll > -90.0f) {
            Roll -= 3.0f;
            updateCameraVectors();
        }
    }
    void dead(bool update) {
        isDead = update;
    }
    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true,
                              GLboolean constraintYaw = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
        if (allowedYaw) {
            Yaw   += xoffset;
        }
        Pitch += yoffset;
        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > PitchClamp[1])
                Pitch = PitchClamp[1];
            if (Pitch < PitchClamp[0])
                Pitch = PitchClamp[0];
        }
        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

    void lockBounds(const std::pair<float, float> bounds[]) {
        float maxX = bounds[0].first, minX = bounds[0].first, maxZ = bounds[0].second, minZ = bounds[0].second;
        for (unsigned int i = 0; i < 4; i++) {
            maxX = bounds[i].first > maxX ? bounds[i].first : maxX;
            minX = bounds[i].first < minX ? bounds[i].first : minX;
            maxZ = bounds[i].second > maxZ ? bounds[i].second : maxZ;
            minZ = bounds[i].second < minZ ? bounds[i].second : minZ;
        }
        this->Position.x = this->Position.x > maxX ? maxX : this->Position.x < minX ? minX : this->Position.x;
        this->Position.z = this->Position.z > maxZ ? maxZ : this->Position.z < minZ ? minZ : this->Position.z;
    }

    void unitCollision(float x, float z) {
        std::vector<std::pair<float, float>> cameraBounds{};
        std::pair<float, float> l1 = std::make_pair(this->Position.x - 0.5f, this->Position.z + 0.5f); // top left of camera
        std::pair<float, float> r1 = std::make_pair(this->Position.x + 0.5f, this->Position.z - 0.5f); // bottom right of camera
        std::pair<float, float> l2 = std::make_pair(x - 0.5f, z + 0.5f); // top left of object
        std::pair<float, float> r2 = std::make_pair(x + 0.5f, z - 0.5f); // bottom right of object
        if (l1.first < r2.first && l2.first < r1.first && l1.second > r2.second && l2.second > r1.second){
            this->Position = this->previousPosition;
        }
        //if (cTL.first < oRB.first && oTL.first < cRB.first
        //&&  cTL.second > oRB.second && cRB.second > oTL.second) {
        //   this->Position = this->previousPosition;
        //}
        /* this->Position = this->Position.x + 0.5f <= maxX && this->Position.x - 0.5f >= minX 
            && this->Position.z + 0.5f <= maxZ && this->Position.z - 0.5f >= minZ
            ? this->previousPosition : this->Position; */
    }

    void checkForTorch(float torchX, float torchZ) {
        this->hasTorch = this->Position.x <= torchX + 0.5f && this->Position.x >= torchX - 0.5f
            && this->Position.z <= torchZ + 0.5f && this->Position.z >= torchZ - 0.5f
            ? true : this->hasTorch;
    }

    bool torch() {
        return hasTorch;
    }

    // Calculates the front vector from the Camera's (updated) Euler Angles i am very lazy and just want to make it public.
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
        glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(Roll), Front); // https://stackoverflow.com/questions/62493770//how-to-add-roll-in-camera-class
        Up = glm::mat3(roll_mat) * Up;
    }

private:
    bool hasTorch;
    glm::vec3 previousPosition;
};
#endif