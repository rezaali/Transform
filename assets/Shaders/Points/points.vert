#include "uniforms.glsl"

uniform mat4 ciModelViewProjection;

in vec4 position_id;
in vec4 velocity_mass;

uniform float size; //slider:1,10,3

void main( void ) 
{
	gl_Position = ciModelViewProjection * vec4( position_id.xyz, 1.0 ); 
	gl_PointSize = velocity_mass.w * iRenderScale * size;
}
