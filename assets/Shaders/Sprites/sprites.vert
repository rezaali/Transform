#include "uniforms.glsl"

uniform samplerBuffer position_mass;

uniform mat4 ciModelViewProjection;

in vec4	ciPosition;
in vec2 ciTexCoord0;

out highp vec2 vTexCoord;
out float vID;

void main( void )
{        
    int instID = gl_InstanceID;
    vID = float( instID ); 
    vec4 pm = texelFetch( position_mass, instID ); 
    vec3 vPos = 0.5 * ciPosition.xyz + pm.xyz;

    vec4 pos = ciPosition;    
    pos = ciModelViewProjection * vec4( vPos, 1.0 );
	gl_Position	= pos; 
    vTexCoord = ciTexCoord0;         
}