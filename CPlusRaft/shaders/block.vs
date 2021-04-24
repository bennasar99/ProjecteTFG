#version 110

out vec4 vertexPos;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	vertexPos = gl_Position;
	Normal = gl_Normal;  
	FragPos = vec3(gl_ModelViewMatrix * gl_Vertex);
}