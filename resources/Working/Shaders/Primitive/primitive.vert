#include "uniforms.glsl"
#include "pi.glsl"
#include "rotate.glsl"
#include "noise3D.glsl"

uniform samplerBuffer position_id;
uniform samplerBuffer velocity_mass;
uniform samplerBuffer color;
uniform samplerBuffer orientation;

uniform mat4 ciModelViewProjection;
uniform mat4 ciViewMatrix;
uniform mat4 ciInverseViewMatrix;
uniform mat3 ciNormalMatrix;

uniform float radius; //slider

in vec4 ciPosition;
in vec3	ciNormal;
in vec2 ciTexCoord0;

out highp vec2 vTexCoord;
out highp vec3 vNormal;
out highp vec4 vColor;
out highp float vDiffuse;

void main( void )
{
    int instID = gl_InstanceID;
    vec4 pid = texelFetch( position_id, instID );
    float mass = texelFetch( velocity_mass, instID ).w;

    vec3 pos = ciPosition.xyz;
    float offset = mass;
    pos *= ( offset ) * radius * 0.1;

    gl_Position = ciModelViewProjection * vec4( pos.xyz + pid.xyz, 1.0 );

    vTexCoord = ciTexCoord0;
    vNormal = ciNormalMatrix * normalize( ciNormal );
    vDiffuse = dot( vNormal, - mat3( ciViewMatrix ) * iCamViewDir );
    vDiffuse = clamp( vDiffuse, 0.0, 1.0 );
    vColor = texelFetch( color, instID );
}