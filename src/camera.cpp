//
// Created by hhabib on 12/07/2026.
//

#include "../include/camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw = YAW, float pitch = PITCH)
  : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
  this -> position = position;
  this -> worldUp = up;
  this -> pitch = pitch;
  this -> UpdateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch) {
  position = glm::vec3(posX, posY, posZ);
  worldUp = glm::vec3(upX, upY, upZ);
  this -> yaw = yaw;
  this -> pitch = pitch;
  this -> UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
  return glm::lookAt(position, position + front, up);
}

void Camera::ProcessKeyboard(CAMERA_MOVEMENT direction, float deltaTime) {
  float velocity = movementSpeed * deltaTime;

  if (direction == FORWARD)
    position += front * velocity;
  else if (direction == BACKWARD)
    position += -front * velocity;
  else if (direction == RIGHT)
    position += right * velocity;
  else if (direction == LEFT)
    position += -right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
  xoffset *= mouseSensitivity;
  yoffset *= mouseSensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (constrainPitch)
    pitch = std::max(-89.0f, std::min(89.0f, pitch));

  this -> UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
  zoom -= (float)yoffset;
  zoom = std::max(1.0f, std::min(45.0f, zoom));

}

void Camera::UpdateCameraVectors() {
  glm::vec3 frontVec;
  frontVec.x = cos(glm::radians(yaw) * cos(glm::radians(pitch)));
  frontVec.y = sin(glm::radians(pitch));
  frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  frontVec = glm::normalize(frontVec);

  right = glm::normalize(glm::cross(frontVec, worldUp));
  up = glm::normalize(glm::cross(right, front));
}