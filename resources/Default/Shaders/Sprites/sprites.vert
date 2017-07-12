#include "uniforms.glsl"

uniform samplerBuffer position_id;
uniform samplerBuffer velocity_mass;
uniform samplerBuffer color;
uniform samplerBuffer orientation;

uniform mat4 ciModelViewProjection;

in vec4	ciPosition;
in vec2 ciTexCoord0;

out highp vec2 vTexCoord;
out highp float vMass;

void main( void )
{        
    int instID = gl_InstanceID;
    vec3 p = texelFetch( position_id, instID ).xyz; 
    vMass = texelFetch( velocity_mass, instID ).w; 

    vec2 uv = 2.0 * ciTexCoord0 - 1.0;    
    uv.x /= iAspect;   

    vec4 pos = ciModelViewProjection * vec4( ciPosition.xyz + p, 1.0 );
    pos.xy += uv * ( vMass  + 0.5 ) * iRenderScale;
    
	gl_Position	= pos; 
    vTexCoord = ciTexCoord0;      
}