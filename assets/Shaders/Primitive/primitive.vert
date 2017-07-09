#include "uniforms.glsl"

uniform samplerBuffer position_mass;

uniform mat4 ciModelViewProjection;
uniform mat4 ciInverseViewMatrix;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3	ciNormal;
in vec2 ciTexCoord0;

out highp vec2  vTexCoord;
out highp vec3  vNormal;

void main( void )
{    
    int instID = gl_InstanceID;
    vec4 pm = texelFetch( position_mass, instID );         
    gl_Position = ciModelViewProjection * ( vec4( 0.025 * ciPosition.xyz + pm.xyz, 1.0 ) ); 
    vTexCoord = ciTexCoord0;
    vNormal = ciNormal;
}