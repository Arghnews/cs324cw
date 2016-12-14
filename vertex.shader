#version 330 core
layout (location = 0) in vec3 position; // The position variable has attribute position 0
layout (location = 1) in vec3 color;   // The color variable has attribute position 1

uniform float xOffset;

out vec3 ourColor; // Output a color to the fragment shader

void main()
{
    vec3 pos = vec3(position.x+xOffset,position.y,position.z);
    gl_Position = vec4(pos, 1.0);
    ourColor = color; // Set ourColor to the input color we got from the vertex data
}
