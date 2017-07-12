#include "uniforms.glsl"
#include "pi.glsl"
#include "rotate.glsl"

uniform samplerBuffer position_id;
uniform samplerBuffer velocity_mass;
uniform samplerBuffer color;
uniform samplerBuffer orientation;

uniform mat4 ciModelViewProjection;
uniform mat4 ciInverseViewMatrix;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3	ciNormal;
in vec2 ciTexCoord0;

out highp vec2 vTexCoord;
out highp vec3 vPos;
out highp vec3 vNormal;

void main( void )
{    
    int instID = gl_InstanceID;
    vec3 p = texelFetch( position_id, instID ).xyz;
    float mass = texelFetch( velocity_mass, instID ).w; 

    vec3 pos = ciPosition.xyz;     
    mat3 rot = rotationMatrix( vec3( mass, -mass, 0.0 ), 100.0 * mass ); 
    pos *= rot; 
    
    vec4 posf = ciModelViewProjection * ( vec4( 0.195 * mass * pos.xyz + p.xyz, 1.0 ) );  
    gl_Position = posf; 

    vPos = posf.xyz; 
    vTexCoord = ciTexCoord0;    

    vNormal = ciNormal * rot; 
    vNormal = ciNormalMatrix * vNormal;      
}