#include "uniforms.glsl"
#include "pi.glsl"

in vec2	vTexCoord;
in vec3	vNormal;

out vec4 oColor;

void main( void )
{	
	oColor = vec4( vNormal, 1.0 );
}