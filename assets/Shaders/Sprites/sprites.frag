#include "uniforms.glsl"
#include "hsv2rgb.glsl"
#include "pi.glsl"
#include "map.glsl"

uniform vec4 Color; //color

in vec2	vTexCoord;
in float mass;
in vec2 vel;
in vec3 vPos;

out vec4 oColor;

void main( void )
{	
	oColor = Color;
}