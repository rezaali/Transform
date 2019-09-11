
#include "uniforms.glsl"
#include "pi.glsl"
#include "hsv2rgb.glsl"

uniform vec4 Color; //color

in vec2	vTexCoord;
in float vMass;
in vec4 vColor;

uniform float power; //slider
uniform float sigma; //slider

out vec4 oColor;

void main( void )
{
	float dist = length( vTexCoord - vec2( 0.5 ) );
	dist = pow( dist, power );

	float a = 1.0 / ( sigma * sqrt( TWO_PI ) );
	float b = 0.0;
	float gau = a * exp( - pow( dist - b, 2.0 ) / ( 2.0 * pow( sigma, 2.0 ) ) );

	oColor.rgb = Color.rgb * vec3( gau );
	oColor.a = Color.a;


	// oColor = vec4( vTexCoord.x, vTexCoord.y, 0.0, 1.0 );
}