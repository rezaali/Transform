in vec4 plexus;		

uniform samplerBuffer uPositionId;

out vec4 tf_plexus;

void main(void)
{
	int row = int( plexus.y );
	int col = int( plexus.z );

	vec3 p0 = texelFetch( uPositionId, row ).xyz; 	
	vec3 p1 = texelFetch( uPositionId, col ).xyz;

	tf_plexus = plexus;
	tf_plexus.x = length( p0 - p1 ); 
}

/*
					COLS
		___________________________
			0	1	2	3	4	5
		0	-	0	1	2	3	4
		1	-	-	5	6	7	8
ROWS	2	-	-	-	9	10	11
		3	-	-	-	-	12	13
		4	-	-	-	-	-	14
		5	-	-	-	-	-	-

*/