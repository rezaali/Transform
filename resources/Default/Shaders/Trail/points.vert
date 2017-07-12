#version 150 core

#include "uniforms.glsl"

uniform mat4 ciModelViewProjection; 

uniform float pointSize; //slider:1,4,2

in vec3 position;
in ivec4 info;
out vec3 Pos; 
out float ID; 

void main(void)
{
	Pos = position; 
	ID = float(info.x); 
	gl_Position = ciModelViewProjection * vec4( Pos, 1.0 ); 
	gl_PointSize = iRenderScale * pointSize;
}
