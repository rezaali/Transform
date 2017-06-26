#include "uniforms.glsl"
#include "pi.glsl"
#include "hsv2rgb.glsl"

uniform vec4 Color; //color

in vec2	vTexCoord;
in float vID;

out vec4 oColor;

void main( void )
{	
	oColor = Color * vec4( 
		hsv2rgb( 
			iAnimationTime + 2.0 * (vID/iNumParticles), 1.0, 1.0 ), 1.0 );
}