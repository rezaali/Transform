vec3 sphericalForce( in vec3 p, in vec3 center, in float radius )
{	
	vec3 d = p - center; 	
	d = normalize( d ); 
	d *= radius; 
	d += center; 	
	return ( p - d );
}