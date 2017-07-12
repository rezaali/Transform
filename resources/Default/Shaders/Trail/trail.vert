#version 150 core
#extension all : warn

in vec4 position_mass;		//
in ivec4 info;				//ID/#CONS/OFFSET/

//FROM PARTICLES
uniform samplerBuffer tex_particle_positions;
uniform samplerBuffer tex_trails_positions;

uniform int trailLength; 
out vec4 tf_position_mass;

void main(void)
{
	int id = info.x; 	
	if( id % trailLength == 0 ) {
		tf_position_mass = texelFetch( tex_particle_positions, id/trailLength ); 		    
	} else {
		tf_position_mass = texelFetch( tex_trails_positions, id - 1 ); 	
	}
}
	