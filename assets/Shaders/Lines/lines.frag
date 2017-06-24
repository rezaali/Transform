#include "hsv2rgb.glsl"
#include "uniforms.glsl"

uniform vec4 Color; //color

uniform float dist; //slider:0.0,40.0,2.0
uniform float power; //slider:0.0,32.0,10.0

in float vDist; 
in float vMass; 

out vec4 oColor;

void main(void)
{	
	oColor = Color;
	float a = ( dist - clamp( vDist, 0.0,  dist ) ) / ( dist ) ;
	oColor.a *= pow( a, power );
}