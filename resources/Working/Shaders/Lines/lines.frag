#include "hsv2rgb.glsl"
#include "uniforms.glsl"

uniform vec4 Color; //color

uniform float dist; //slider:0.0,4.0,1.0
uniform float power; //slider:0.0,32.0,10.0

in float vDist;
in vec4 vColor;

out vec4 oColor;

void main(void)
{
	oColor = vColor * Color;
	float a = ( dist - clamp( vDist, 0.0,  dist ) ) / ( dist ) ;
	oColor.a *= pow( a, power );
}