//
// Created by hhabib on 31/07/2026.
//

#include "core/Transform.h"
#include <glm/gtx/quaternion.hpp>

Transform::Transform()
    : position(0.0f),
      rotation(glm::identity<glm::quat>()),
      scale(1.0f)
{
}

glm::mat4 Transform::getModelMatrix() const {
  glm::mat4 model(1.0f);
  model = glm::translate(model, position);
  model *= glm::toMat4(rotation);
  model = glm::scale(model, scale);
  return model;
}
