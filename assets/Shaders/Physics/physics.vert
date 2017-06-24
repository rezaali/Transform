#version 330 
#include "uniforms.glsl"
#include "pi.glsl"
#include "limit.glsl"
#include "noise2D.glsl"
#include "damping.glsl"
#include "spherical.glsl"
#include "attract.glsl"

in vec4 position_mass;		
in vec4 velocity_mass;		
in ivec4 info;				

uniform samplerBuffer uPositionMass;
uniform samplerBuffer uVelocity;

out vec4 tf_position_mass;
out vec4 tf_velocity_mass;
out ivec4 tf_info;


vec3 collisionForce( in vec4 pm, int id, int total, in samplerBuffer positionmass ) {
    float count = 1.0;       
    vec3 result = vec3( 0.0 );        
    for ( int i = 0; i < total; i++ ) {                    
        if( i != id ) {
            vec4 other = texelFetch( positionmass, i );                
            vec3 direction = other.xyz - pm.xyz;              
            float dist = length( direction );            
            if( dist < 0.5 * (other.w + pm.w) ) {
            	result -= other.w * pm.w * ( direction / ( dist * dist ) );                                    
            	count++;                       
            }                                          
        }
    }
    return result / count;     
}

void main()
{
	vec3 pos = position_mass.xyz; 
	float mass = position_mass.w; 
	vec3 vel = velocity_mass.xyz; 

	pos = vec3( 0.0 ); 
	mass = 3.0; 

	tf_position_mass = vec4( pos, mass );
	tf_velocity_mass = vec4( vel, mass );
	tf_info = info; 
}