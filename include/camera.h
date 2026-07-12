//
// Created by hhabib on 12/07/2026.
//

#ifndef PROCEDURALPLANETOPENGL_CAMERA_H
#define PROCEDURALPLANETOPENGL_CAMERA_H

#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


enum CAMERA_MOVEMENT {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera {
public:
  glm::vec3 position, front, up, right, worldUp;

  float yaw, pitch;

  float movementSpeed, mouseSensitivity, zoom;

  Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);


  glm::mat4 GetViewMatrix();
  void ProcessKeyboard(CAMERA_MOVEMENT direction, float deltaTime);
  void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
  void ProcessMouseScroll(float yoffset);

private:
  void UpdateCameraVectors();
};

#endif  // PROCEDURALPLANETOPENGL_CAMERA_H
