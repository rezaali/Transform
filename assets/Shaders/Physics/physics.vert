#version 330 
#include "uniforms.glsl"
#include "pi.glsl"
#include "random.glsl"
#include "noise3D.glsl"

in vec4 position_id;		
in vec4 velocity_mass;		
in vec4 color;
in vec4 orientation;      

uniform samplerBuffer uPositionId;
uniform samplerBuffer uVelocityMass;

uniform float amplitude; //slider:0.0,1.0,0.25
uniform float radius; //slider:0.0,1.0,0.25

out vec4 tf_position_id;
out vec4 tf_velocity_mass;
out vec4 tf_color;
out vec4 tf_orientation;

void main()
{
	vec3 pos = position_id.xyz; 	
	vec3 vel = velocity_mass.xyz; 
    float mass = velocity_mass.w; 

    int id = int( position_id.w ); 
    float idf = float( id ); 

    int total = int( sqrt( iNumParticles ) );
    float totalf = float( total ); 
    
    int row = id % total;  
    int col = int( floor( id / total ) ); 

    float norm = idf / iNumParticles; 
    
    float x = radius * sin( norm * TWO_PI ); 
    float z = radius * cos( norm * TWO_PI );     
    float y = 0.2 + amplitude * sin( norm * TWO_PI * 4.0 + 10.0 * iAnimationTime * TWO_PI ); 
		
    pos = vec3( x, y, z ); 
    mass = 2.5 * amplitude * sin( norm * TWO_PI * 4.0 - 10.0 * iAnimationTime * TWO_PI ); 

	tf_position_id = vec4( pos, position_id.w );
	tf_velocity_mass = vec4( vel, mass );
	tf_color = color; 
    tf_orientation = orientation; 
}