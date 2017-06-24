#include "uniforms.glsl"

uniform mat4 ciModelViewProjection;

in vec4 position_mass;
in vec4 velocity_mass;

uniform float size; //slider:1,10,3

out float mass; 
void main( void ) 
{
	gl_Position = ciModelViewProjection * vec4( position_mass.xyz, 1.0 ); 
	gl_PointSize = iRenderScale * size;
	mass = position_mass.w; 
}
