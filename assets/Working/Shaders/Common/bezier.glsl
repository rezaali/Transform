vec4 bezier( float t, in vec4 p0, in vec4 p1, in vec4 p2, in vec4 p3 ) {
    float omt = 1.0 - t;
    float omt2 = pow( omt, 2.0 );
    float omt3 = pow( omt, 3.0 );
    float t2 = pow( t, 2.0 );
    float t3 = pow( t, 3.0 );
    return omt3 * p0 + 3.0 * omt2 * t * p1 + 3.0 * omt * t2 * p2 + t3 * p3;
}

vec3 bezier( float t, in vec3 p0, in vec3 p1, in vec3 p2, in vec3 p3 ) {
    float omt = 1.0 - t;
    float omt2 = pow( omt, 2.0 );
    float omt3 = pow( omt, 3.0 );
    float t2 = pow( t, 2.0 );
    float t3 = pow( t, 3.0 );
    return omt3 * p0 + 3.0 * omt2 * t * p1 + 3.0 * omt * t2 * p2 + t3 * p3;
}

vec2 bezier( float t, in vec2 p0, in vec2 p1, in vec2 p2, in vec2 p3 ) {
    float omt = 1.0 - t;
    float omt2 = pow( omt, 2.0 );
    float omt3 = pow( omt, 3.0 );
    float t2 = pow( t, 2.0 );
    float t3 = pow( t, 3.0 );
    return omt3 * p0 + 3.0 * omt2 * t * p1 + 3.0 * omt * t2 * p2 + t3 * p3;
}

float bezier( float t, float p0, float p1, float p2, float p3 ) {
    float omt = 1.0 - t;
    float omt2 = pow( omt, 2.0 );
    float omt3 = pow( omt, 3.0 );
    float t2 = pow( t, 2.0 );
    float t3 = pow( t, 3.0 );
    return omt3 * p0 + 3.0 * omt2 * t * p1 + 3.0 * omt * t2 * p2 + t3 * p3;
}

vec3 bezier(float t, vec3 a, vec3 b, vec3 c) {
    float OneMinusT = (1.0 - t);
    vec3 a0 = 1.0 * pow(t, 0.0) * pow(OneMinusT, 2.0) * a;
    vec3 b0 = 2.0 * pow(t, 1.0) * pow(OneMinusT, 1.0) * b;
    vec3 c0 = 1.0 * pow(t, 2.0) * pow(OneMinusT, 0.0) * c;
    return a0 + b0 + c0;
}
