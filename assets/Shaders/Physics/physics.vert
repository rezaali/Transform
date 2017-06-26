#version 330 
#include "uniforms.glsl"
#include "pi.glsl"

in vec4 position_mass;		
in vec4 velocity_mass;		
in ivec4 info;				

uniform samplerBuffer uPositionMass;
uniform samplerBuffer uVelocity;

out vec4 tf_position_mass;
out vec4 tf_velocity_mass;
out ivec4 tf_info;

void main()
{
	vec3 pos = position_mass.xyz; 
	float mass = position_mass.w; 
	vec3 vel = velocity_mass.xyz; 

    int id = info.x % 2; 
    float idf = float( id ); 
    float sdf = mix( 1, -1, idf ); 

    float norm = float( info.x ) / float( iNumParticles ) - 0.5; 
	pos = vec3(     
        3.0 * norm , 
        sdf * 1.5 * sin( 2.0 * norm * TWO_PI + 2.0 * iAnimationTime * TWO_PI ), 
        sdf * 1.5 * cos( 2.0 * norm * TWO_PI + 2.0 * iAnimationTime * TWO_PI ) ); 
	
    mass = 3.0; 

	tf_position_mass = vec4( pos, mass );
	tf_velocity_mass = vec4( vel, mass );
	tf_info = info; 
}