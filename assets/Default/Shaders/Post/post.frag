#include "uniforms.glsl"

uniform sampler2D iScreenTexture; 

uniform bool invert; //toggle

in vec2 vTexcoord;

out vec4 oColor;

void main(void)
{
	vec2 sUV = gl_FragCoord.xy / iResolution.xy; 		
	vec4 color = texture( iScreenTexture, sUV ); 
 	vec4 inv = vec4( 1, 1, 1, 2 ) - color; 
  	oColor = mix( color, inv, int( invert ) );    	
}