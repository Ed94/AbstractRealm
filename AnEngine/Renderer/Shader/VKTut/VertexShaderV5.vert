#version 450

#extension GL_ARB_separate_shader_objects : enable



layout(binding = 0) uniform UniformBufferObject 
{
    mat4 ModelSpace;
    mat4 Viewport  ;
    mat4 Projection;
} UBO;

layout(location = 0) in vec3 VertexPosition;

layout(location = 1) in vec3 VertexColor;

layout(location = 2) in vec2 VertTextureCoordinate;

layout(location = 0) out vec3 FragColor;

layout(location = 1) out vec2 FragTextureCoordinate;



void main() 
{
    gl_Position = UBO.Projection * UBO.Viewport * UBO.ModelSpace * vec4(VertexPosition, 1.0);

    FragColor = VertexColor;

    FragTextureCoordinate = VertTextureCoordinate;
}