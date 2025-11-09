#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 ourColor;
uniform mat4 transform;
uniform mat4 worldTransform;

void main()
{
   gl_Position = transform * worldTransform * vec4(aPos.x, -aPos.y, aPos.z, 1.0);
   ourColor = aColor;
}