#version 330 
#include "uniforms.glsl"
#include "pi.glsl"
#include "noise3D.glsl"

in vec4 position_mass;		
in vec4 velocity_mass;		
in ivec4 info;				

uniform samplerBuffer uPositionMass;
uniform samplerBuffer uVelocity;

uniform float amplitude; //slider:0.0,1.0,0.25
uniform float speed; //slider:0.0,2.0,0.5
uniform vec2 pad; //pad:-1.0,1.0,0.0
uniform bool activate; //button:0

out vec4 tf_position_mass;
out vec4 tf_velocity_mass;
out ivec4 tf_info;

void main()
{
	vec3 pos = position_mass.xyz; 
	float mass = position_mass.w; 
	vec3 vel = velocity_mass.xyz; 

    int id = info.x; 
    float idf = float( id ); 

    int total = int( sqrt( iNumParticles ) );
    float totalf = float( total ); 
    
    int row = id % total;  
    int col = int( floor( id / total ) ); 

    float norm = idf / iNumParticles; 

    float x = float( row ) / ( totalf - 1 ); 
    float z = float( col ) / ( totalf - 1 ); 
    float y = ( activate ? -1.0 : 1.0 ) * amplitude * snoise( vec3( x, z, speed * iGlobalTime ) ); 
	
	float amp = texture( iAmplitude, vec2( norm/4, 0.0 ) ).r; 	
    pos = vec3( x - 0.5 + pad.x, y, z - 0.5 - pad.y );

    mass = y * amp; 

	tf_position_mass = vec4( pos, mass );
	tf_velocity_mass = vec4( vel, mass );
	tf_info = info; 
}


/*

Rect 
RoundedRect
Cube
Icosahedron
Icosphere
Teapot
Circle
Ring
Sphere
Capsule
Torus
TorusKnot
Helix
Cylinder
Cone
Plane

*/