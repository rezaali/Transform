
#include "uniforms.glsl"
#include "pi.glsl"
#include "hsv2rgb.glsl"

uniform vec4 Color; //color

in vec2	vTexCoord;
in float vMass;

uniform float power; //slider
uniform float sigma; //slider
uniform int palette; //dialer:0,992,0

out vec4 oColor;

void main( void )
{		
	float dist = length( vTexCoord - vec2( 0.5 ) );
	dist = pow( dist, power ); 

	float a = 1.0 / ( sigma * sqrt( TWO_PI ) );
	float b = 0.0; 
	float gau = a * exp( - pow( dist - b, 2.0 ) / ( 2.0 * pow( sigma, 2.0 ) ) );
	
	float lookup = fract( abs( vMass ) );
	float pIndex = float( palette ) / 992.0; 

	oColor = Color * texture( iPalettes, vec2( lookup, pIndex ) );
	// oColor.rgb = vec3( 1.0 ) - hsv2rgb( vec3( vMass, 1.0, 1.0 ) ); 
	oColor *= vec4( gau ); 	
}