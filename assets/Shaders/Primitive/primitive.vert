#include "uniforms.glsl"
#include "pi.glsl"
#include "rotate.glsl"

uniform samplerBuffer position_mass;

uniform mat4 ciModelViewProjection;
uniform mat4 ciInverseViewMatrix;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3	ciNormal;
in vec2 ciTexCoord0;

out highp vec2  vTexCoord;
out highp vec3  vPos;
out highp vec3  vNormal;

void main( void )
{    
    int instID = gl_InstanceID;
    vec4 pm = texelFetch( position_mass, instID );       
    vec3 pos = ciPosition.xyz;     
    mat3 rot = rotationMatrix( vec3( pm.w, -pm.w, 0.0 ), 100.0 * pm.w ); 
    pos *= rot; 
    
    vec4 posf = ciModelViewProjection * ( vec4( 0.25 * pm.w * pos.xyz + pm.xyz, 1.0 ) );  
    gl_Position = posf; 
    vPos = posf.xyz; 
    vTexCoord = ciTexCoord0;

    vNormal = ciNormal * rot; 
    vNormal = ciNormalMatrix * vNormal;      
}