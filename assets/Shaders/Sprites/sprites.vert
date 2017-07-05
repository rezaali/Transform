#include "uniforms.glsl"

uniform samplerBuffer position_mass;

uniform mat4 ciModelViewProjection;

in vec4	ciPosition;
in vec2 ciTexCoord0;

out highp vec2 vTexCoord;
out highp float vMass;

void main( void )
{        
    int instID = gl_InstanceID;
    vec4 pm = texelFetch( position_mass, instID ); 
    vec2 uv = 2.0 * ciTexCoord0 - 1.0;    
    uv.x /= iAspect;   
    vec4 pos = ciModelViewProjection * vec4( ciPosition.xyz + pm.xyz, 1.0 );
    pos.xy += uv * ( pm.w + 0.1 ) * iRenderScale;
    vMass = pm.w;
	gl_Position	= pos; 
    vTexCoord = ciTexCoord0;      
}