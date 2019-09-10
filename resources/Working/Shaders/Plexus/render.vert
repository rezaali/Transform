uniform mat4 ciModelViewProjection;

uniform samplerBuffer uPositionMass;
uniform samplerBuffer uPlexus;
in vec4	ciPosition;

out float vDist; 

void main( void ) 
{
	int id = int( ciPosition.w );
	int index = id % 2; 
	id -= index;

	vec4 plexus = texelFetch( uPlexus, id );
	vec3 p0 = texelFetch( uPositionMass, int( plexus.y ) ).xyz; 	
	vec3 p1 = texelFetch( uPositionMass, int( plexus.z ) ).xyz; 	
	vDist = plexus.x; 

	gl_Position = ciModelViewProjection * vec4( mix( p0, p1, float(index) ), 1.0 );
}
