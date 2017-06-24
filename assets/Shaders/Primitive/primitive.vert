#include "uniforms.glsl"
#include "pi.glsl"
#include "noise4D.glsl"
#include "map.glsl"
#include "rotate.glsl"
#include "limit.glsl"

uniform samplerBuffer position_mass;

uniform mat4 ciModelViewProjection;
uniform mat4 ciInverseViewMatrix;

uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3	ciNormal;
in vec2 ciTexCoord0;

out highp vec2  vTexCoord;
out highp vec3  vNormal;

uniform float noiseScale; //slider
uniform float amplitude; //slider
uniform vec2 er; //range

out float vID; 
out float vDisp; 
out vec3 vPos;
out vec4 vPosf;
out vec3 vEye;
out vec3 vView;  

void main( void )
{    
    int instID = gl_InstanceID;
    vec4 pm = texelFetch( position_mass, instID ); 
    vPos = abs( pm.w ) * ciPosition.xyz + pm.xyz;

    float time = sin( iAnimationTime * TWO_PI ); 

    vDisp = amplitude * snoise( 
        vec4( vec3( noiseScale * vPos ), 
        time ) ); 

    float div = 10; 
    vDisp = floor( vDisp * div );
    vDisp = map( vDisp, 0, div, er.x, er.y ); 


    vec3 push = normalize( vPos ) * vDisp; 
    vPos += push; 
    mat3 mtx = rotationMatrix( vec3( time, 1, 1 - time ), TWO_PI * iAnimationTime ); 
    vPos = mtx * vPos; 
    // if( vDisp < 0 ) {
        // disgard; 
    // }
    vPosf = ciModelViewProjection * ( vec4( vPos, 1.0 ) );
    gl_Position = vPosf; 
    vTexCoord = ciTexCoord0;

    vNormal = mtx * ciNormal;
    vNormal = ciNormal;
    vID = float( instID ); 
    vEye = iCamEyePos; 
    vView = iCamViewDir; 
}