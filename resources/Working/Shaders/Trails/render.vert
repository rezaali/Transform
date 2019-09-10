#version 150 core

uniform mat4 uModelViewProjection; 

in vec3 position;
in ivec4 info;
out vec3 Pos; 
out float ID; 

void main(void)
{
	Pos = position; 
	ID = float(info.x); 
	gl_Position = uModelViewProjection * vec4( Pos, 1.0 ); 
}
