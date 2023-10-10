#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

inline glm::mat4 TransformationMatrix(glm::vec2 Translation = { 0.0f, 0.0f }, float Rotation = 0.0f, glm::vec2 Scale = { 1.0f, 1.0f })
{
	auto TransformationMatrix = glm::mat4(1.0f);
	TransformationMatrix = glm::translate(TransformationMatrix, glm::vec3(Translation, 0.0f));
	TransformationMatrix = glm::rotate(TransformationMatrix, Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	TransformationMatrix = glm::scale(TransformationMatrix, glm::vec3(Scale, 1.0f));
	return TransformationMatrix;
}
