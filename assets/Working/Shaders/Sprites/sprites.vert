#include "uniforms.glsl"

uniform samplerBuffer position_id;
uniform samplerBuffer velocity_mass;
uniform samplerBuffer color;
uniform samplerBuffer orientation;

uniform mat4 ciProjectionMatrix;
uniform mat4 ciModelMatrix;
uniform mat4 ciViewMatrix;


in vec4	ciPosition;
in vec2 ciTexCoord0;

out highp vec2 vTexCoord;
out highp float vMass;
out highp vec4 vColor;

void main( void )
{
    int instID = gl_InstanceID;
    vec3 p = texelFetch( position_id, instID ).xyz;
    vMass = texelFetch( velocity_mass, instID ).w;

    vec2 uv = 2.0 * ciTexCoord0 - 1.0;
    // uv *= 0.5;

    vec3 pp = ciPosition.xyz + p;
    vec4 pos = ciModelMatrix * vec4( pp, 1.0 );
    pos = ciViewMatrix * pos;
    pos.xy += 0.0125 * vMass * uv * iRenderScale;
    pos = ciProjectionMatrix * pos;


	gl_Position	= pos;
    vTexCoord = ciTexCoord0;
    vColor = texelFetch( color, instID );
}