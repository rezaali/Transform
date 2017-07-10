#include "uniforms.glsl"
#include "hsv2rgb.glsl"
#include "pi.glsl"

in vec2	vTexCoord;
in vec3	vNormal;
in vec3	vPos;

uniform float palette; //slider:0,992,0
out vec4 oColor;

void main( void )
{		
	float dp = dot( vNormal, vec3( 0, 0, 1 ) ); 
	float dist = length( vPos ); 

	oColor.a = 1.0; 
	oColor.rgb = hsv2rgb( 0.2 * dist + 0.125 + 0.1 * dp * dist + vNormal.x * 0.01 + vNormal.y * 0.01 + vNormal.z * 0.01, 1.0, 1.0 ); 
	oColor.rgb = mix( oColor.rgb, iBackgroundColor.rgb, 1.0 - dp ); 
	// oColor.rgb = mix( oColor.rgb, iBackgroundColor.rgb, dist - 1.0 );
}