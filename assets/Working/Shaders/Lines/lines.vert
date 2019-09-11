uniform mat4 ciModelViewProjection;

uniform samplerBuffer position_id;
uniform samplerBuffer velocity_mass;
uniform samplerBuffer color;
uniform samplerBuffer orientation;

uniform samplerBuffer uPlexus;

in vec4	ciPosition;

out float vDist;
out vec4 vColor;

void main( void )
{
	int id = int( ciPosition.w );
	int index = id % 2;
	id -= index;

	vec4 plexus = texelFetch( uPlexus, id );

	vec3 p0 = texelFetch( position_id, int( plexus.y ) ).xyz;
	vec3 p1 = texelFetch( position_id, int( plexus.z ) ).xyz;

    vec4 v0 = texelFetch( velocity_mass, int( plexus.y ) );
    vec4 v1 = texelFetch( velocity_mass, int( plexus.z ) );

    vDist = plexus.x;

    vColor = texelFetch( color, int( plexus.z ) );
    vColor.a *= v0.a * v1.a;

	gl_Position = ciModelViewProjection * vec4( mix( p0, p1, float(index) ), 1.0 );
}
