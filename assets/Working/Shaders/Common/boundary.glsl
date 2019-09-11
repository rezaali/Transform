float boundary( in float pos, in float limit ) {
	if( pos > limit ) {
		pos = limit;
	}
	else if( pos < -limit ) {
		pos = -limit;
	}
	return pos;
}

vec3 boundary( in vec3 pos, in float limit ) {
	pos.x = boundary( pos.x, limit );
	pos.y = boundary( pos.y, limit );
	pos.z = boundary( pos.z, limit );
	return pos;
}

vec3 boundary( in vec3 pos, in vec3 limit ) {
	pos.x = boundary( pos.x, limit.x );
	pos.y = boundary( pos.y, limit.y );
	pos.z = boundary( pos.z, limit.z );
	return pos;
}