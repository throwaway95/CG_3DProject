#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
out vec3 TexCoord;
out vec3 fragmentColor;

void main(){
	TexCoord = vertexPosition_modelspace;
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	vec4 position = vec4(vertexPosition_modelspace,1.0);

	float factor = 0.1;
    fragmentColor = vec3(position.y*factor,position.y*factor,position.y*factor);
    
}

