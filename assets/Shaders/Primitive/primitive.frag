#include "uniforms.glsl"
#include "pi.glsl"
#include "map.glsl"
#include "hsv2rgb.glsl"

in vec2	vTexCoord;
in vec3	vNormal;

uniform vec2 cr; //range
in float vID; 
in float vDisp; 
in vec3 vPos;
in vec3 vEye; 
in vec3 vView; 
in vec4 vPosf;

out vec4 oColor;

void main( void )
{	
	oColor = vec4( 1.0 ); 
	vec3 delta = normalize( vEye - vView );
	float rim = clamp( dot( delta, vNormal ), 0.0, 1.0 );

	float div = 40; 
	float d = floor( vDisp * div ); 

	// oColor = vec4( pow( rim, 2.0 ) );  
	oColor.rgb = hsv2rgb( 
		map( d + sin( iAnimationTime * TWO_PI ) * div, 0, div, cr.x, cr.y ),
		1.0, 
		1.0 
	); 

	oColor.rgb = vec3( length( oColor.r ) ); 
}