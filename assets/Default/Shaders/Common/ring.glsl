vec3 ringForce( in vec3 p, in vec3 center, in vec3 normal, in float radius )
{
	vec3 deltaPos = p - center;
    float deltaPosLength = length( deltaPos );
    float dotp = dot( ( deltaPos / deltaPosLength ), normal );
    float delta = deltaPosLength * dotp;
    vec3 pointOnPlane = p - delta * normal;
    vec3 pointOnRing = pointOnPlane - center;
    pointOnRing = normalize( pointOnRing ) * radius + center;
	return ( p - pointOnRing );
}