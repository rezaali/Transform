float limit( float p, float limit ) {
	if( p > limit ) {
		p = limit; 
	}
	else if( p < -limit ) {
		p = -limit; 
	}
	return p; 
}

vec3 limit3( in vec3 p, float limit ) {
	p.x = clamp( p.x, -limit, limit ); 
	p.y = clamp( p.y, -limit, limit ); 
	p.z = clamp( p.z, -limit, limit ); 
	return p; 
}