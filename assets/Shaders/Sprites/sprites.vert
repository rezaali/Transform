#include "uniforms.glsl"

uniform mat4	ciModelViewProjection;
uniform mat4	ciModelMatrix;
uniform mat4	ciViewMatrix;
uniform mat4	ciProjectionMatrix;
uniform ivec2   ciWindowSize; 
uniform float   ciElapsedSeconds; 

uniform vec2 scale; //slider:0,1,0.1
in vec4	ciPosition;
in vec2 ciTexCoord0;
in vec4 position_mass; // per-instance position variable
in vec4 velocity_mass; // per-instance position variable

out float mass;
out vec2 vel;  
out vec3 vPos;  
out highp vec2  vTexCoord;

void main( void )
{    
    float aspect = float(ciWindowSize.x)/float(ciWindowSize.y); 

    vec4 offsetPos = vec4(ciTexCoord0 - 0.5, 0.0, 0.0);     
    offsetPos.x /= aspect; 
    offsetPos.xy *= iRenderScale;
	offsetPos.xy *= vec2( scale.x, scale.y ) ;
    // offsetPos.y += position_mass.w;

    vec4 pos = ciPosition;    
    vPos = pos.xyz + position_mass.xyz;
    vPos *= 0.5;
    pos = ciModelViewProjection * ( vec4( vPos, 1.0 ) ); 
    pos += offsetPos;

	gl_Position	= pos; 
    vTexCoord = ciTexCoord0;     
    mass = position_mass.w;
    vel = velocity_mass.xy; 
}