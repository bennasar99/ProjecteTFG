
#version 110

in vec4 vertexPos;
in vec3 lightColor;

void main (void)  
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
	//gl_FragColor = vec4(1, 1, 1, 0.75);
    vec3 result = ambient * gl_Color;
    gl_FragColor = vec4(result, 1.0);
}
