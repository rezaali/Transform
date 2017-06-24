uniform mat4 ciModelViewProjection;

uniform samplerBuffer uPositionMass;
uniform samplerBuffer uPlexus;
in vec4	ciPosition;

out float vDist; 
out float vMass; 

void main( void ) 
{
	int id = int( ciPosition.w );
	int index = id % 2; 
	id -= index;

	vec4 plexus = texelFetch( uPlexus, id );
	vec4 p = texelFetch( uPositionMass, int( plexus.y ) ); 	
	vec3 p0 = p.xyz; 	

	vec3 p1 = texelFetch( uPositionMass, int( plexus.z ) ).xyz; 	
	vDist = plexus.x; 
	vMass = p.w; 

	gl_Position = ciModelViewProjection * vec4( mix( p0, p1, float(index) ), 1.0 );
}
