float linear( float t, float b, float c, float d )
{
    return t * ( c / d ) + b;
}

float linear( float t )
{
    return t;
}

float inQuad( float t, float b, float c, float d )
{
    return c * ( t /= d ) * t + b;
}

float inQuad( float t )
{
    return t * t;
}

float outQuad( float t, float b, float c, float d )
{
    return -c * ( t /= d ) * ( t - 2.0 ) + b;
}

float outQuad( float t )
{
    return - ( t -= 1.0 ) * t + 1.0;
}

float inOutQuad( float t, float b, float c, float d )
{
    if( ( t /= d / 2.0 ) < 1.0 ) {
      return c / 2.0 * t * t + b;
    }
    return - c / 2.0 * ( ( --t ) * ( t - 2.0 ) - 1.0 ) + b;
}

float inOutQuad( float t )
{
    if( ( t /= 0.5 ) < 1.0 ) return 0.5 * t * t;
    return -0.5 * ( ( --t ) * ( t-2 ) - 1 );
}

float inCubic( float t, float b, float c, float d )
{
    return c * ( t /= d ) * t * t + b;
}

float inCubic( float t )
{
    return t * t * t;
}

float outCubic( float t, float b, float c, float d )
{
    return c * ( ( t = t/d - 1.0 ) * t * t + 1.0 ) + b;
}

float outCubic( float t )
{
    return ( ( --t ) * t * t + 1.0 );
}

float inOutCubic( float t, float b, float c, float d )
{
    if( ( t /= d / 2.0 ) < 1.0 ) return  c / 2.0 * t * t * t + b;
    return c / 2.0 * ( ( t -= 2.0 ) * t * t + 2.0 ) + b;
}

float inOutCubic( float t )
{
    if( ( t /= 0.5 ) < 1.0 ) return 0.5 * t * t * t;
    return 0.5 * ( ( t -= 2.0 ) * t * t + 2.0 );
}

float inQuart( float t, float b, float c, float d )
{
    return c * ( t /= d ) * t * t * t + b;
}

float inQuart( float t )
{
    return t * t * t * t;
}

float outQuart( float t, float b, float c, float d )
{
    return -c * ( ( t = t/d - 1.0 ) * t * t * t - 1.0 ) + b;
}

float outQuart( float t )
{
    return - ( ( --t ) * t * t * t - 1.0 );
}

float inOutQuart( float t, float b, float c, float d )
{
    if ( ( t /= d / 2.0 ) < 1.0 ) return c / 2.0 * t * t * t * t + b;
    return -c / 2.0 * ( ( t -= 2.0 ) * t * t * t - 2.0 ) + b;
}

float inOutQuart( float t )
{
    if ( (t /= 0.5 ) < 1.0 ) return 0.5 * t * t * t * t;
    return -0.5 * ( ( t -= 2.0 ) * t * t * t - 2.0 );
}

float inQuint( float t, float b, float c, float d )
{
    return c * ( t /= d ) * t * t * t * t + b;
}

float inQuint( float t )
{
    return t * t * t * t * t;
}

float outQuint( float t, float b, float c, float d )
{
    return c * ( ( t = t/d - 1.0) * t * t * t * t + 1.0 ) + b;
}

float outQuint( float t )
{
    return ( ( --t ) * t * t * t * t + 1.0 );
}

float inOutQuint( float t, float b, float c, float d )
{
    if( ( t /= d /2.0 ) < 1.0 ) return  c / 2.0 * t * t * t * t * t + b;
    return c / 2.0 * ( ( t -= 2.0 ) * t * t * t * t + 2.0) + b;
}

float inOutQuint( float t )
{
    if ( ( t /= 0.5 ) < 1.0 ) return 0.5 * t * t * t * t * t;
    return 0.5 * ( ( t -= 2 ) * t * t * t * t + 2.0 );
}

float inSine( float t, float b, float c, float d )
{
    return -c * cos( t / d * ( 1.5707963268 ) ) + c + b;
}

float inSine( float t )
{
    return -1.0 * cos( t * ( 1.5707963268 ) ) + 1.0;
}

float outSine( float t, float b, float c, float d )
{
    return c * sin( t / d * ( 1.5707963268 ) ) + b;
}

float outSine( float t )
{
    return sin( t * ( 1.5707963268 ) );
}

float inOutSine( float t, float b, float c, float d )
{
    return - c / 2.0 * ( cos( 3.1415926536 * t / d ) - 1.0 ) + b;
}

float inOutSine( float t )
{
    return -0.5 * ( cos( 3.1415926536 * t ) - 1.0 );
}

float inExpo( float t, float b, float c, float d )
{
    return ( t == 0.0 ) ? b : c * pow( 2.0, 10.0 * ( t / d - 1.0 ) ) + b;
}

float inExpo( float t )
{
    return ( t == 0 ) ? 0.0 : pow( 2.0, 10.0 * ( t - 1.0 ) );
}

float outExpo( float t, float b, float c, float d )
{
    return ( t == d ) ? b + c : c * ( - pow( 2.0, -10.0 * t / d ) + 1.0 ) + b;
}

float outExpo( float t )
{
    return ( t == 1.0 ) ? 1.0 : ( - pow( 2.0, -10.0 * t ) + 1.0 );
}

float inOutExpo( float t, float b, float c, float d )
{
    if( t == 0 ) return b;
    if( t == d ) return b + c;
    if(( t /= d / 2.0 ) < 1.0 ) return c / 2.0 * pow( 2.0, 10.0 * ( t - 1.0 ) ) + b;
    return c / 2.0 * ( - pow( 2.0, -10.0 * --t ) + 2.0 ) + b;
}

float inOutExpo( float t )
{
    if( t == 0.0 ) return 0.0;
    if( t == 1.0 ) return 1.0;
    if( ( t /= 0.5 ) < 1.0 ) return 0.5 * pow( 2.0, 10.0 * ( t - 1.0 ) );
    return 0.5 * ( - pow( 2.0, -10.0 * --t ) + 2.0 );
}

float inCirc( float t, float b, float c, float d )
{
    return -c * ( sqrt( 1.0 - (t/=d)*t) - 1) + b;
}

float inCirc( float t )
{
    return - ( sqrt( 1.0 - t*t) - 1);
}

float outCirc( float t, float b, float c, float d )
{
    return c * sqrt( 1.0 - (t=t/d-1)*t) + b;
}

float outCirc( float t )
{
    return sqrt( 1.0 - (--t)*t);
}

float inOutCirc( float t, float b, float c, float d )
{
    if ( ( t /= d / 2.0 ) < 1 ) return - c / 2.0 * ( sqrt( 1.0 - t * t ) - 1.0 ) + b;
    return c / 2.0 * ( sqrt( 1.0 - ( t -= 2.0 ) * t ) + 1.0 ) + b;
}

float inOutCirc( float t )
{
    if( ( t /= 0.5 ) < 1.0 ) return -0.5 * ( sqrt( 1.0 - t * t ) - 1.0 );
    return 0.5 * ( sqrt( 1.0 - ( t -= 2.0 ) * t ) + 1.0 );
}

float inElastic( float t, float b, float c, float d )
{
    float s = 1.70158; float p = 0.0; float a = c;
    if( t == 0 ) return b;  if( ( t /= d ) == 1 ) return b + c;
    p = d * 0.3;
    if( a < abs( c ) ) { a = c; float s = p / 4.0; }
    else s = 0.1591549431 * p / ( 6.2831853072 ) * asin( c / a );
    return -( a * pow( 2.0, 10.0 * ( t -= 1.0 ) ) * sin( ( t * d - s ) * ( 6.2831853072 ) / p ) )  + b;
}

float inElastic( float t )
{
    float s = 1.70158; float p = 0.0; float a = 1.0;
    if( t == 0.0 ) return 0.0;
    if( t == 1.0 ) return 1.0;
    p = 0.3;
    s = p / ( 6.2831853072 ) * asin( 1.0 / a );
    return -( a * pow( 2.0, 10.0 * ( t -= 1.0 ) ) * sin( ( t - s ) * ( 6.2831853072 ) / p ) );
}

float outElastic( float t, float b, float c, float d )
{
    float s = 1.70158; float p = 0.0; float a = c;
    if( t == 0.0 ) return b;
    if( (t /= d ) == 1.0 ) return b + c;
    p = d * 0.3;
    if( a < abs( c ) ) { a = c; s = p / 4.0; }
    else { s = p / ( 6.2831853072 ) * asin( c / a ); }
    return a * pow( 2.0, -10.0 * t ) * sin( ( t * d - s ) * ( 6.2831853072 ) / p ) + c + b;
}

float outElastic( float t )
{
    float s = 1.70158; float p = 0.0 ; float a = 1.0;
    if( t == 0.0 ) return 0.0;  if( t == 1.0 ) return 1.0;
    p = 0.3;
    s = p / ( 6.2831853072 ) * asin( 1.0 / a );
    return a * pow( 2.0, -10.0 * t ) * sin( ( t - s ) * ( 6.2831853072 ) / p ) + 1.0;
}

float inOutElastic( float t, float b, float c, float d )
{
    float s = 1.70158; float p = 0.0; float a = c;
    if( t == 0.0 ) return b;
    if( ( t /= d / 2.0 ) == 2.0 ) return b + c;
    p = d * ( 0.3 * 1.5 );
    if( a < abs( c ) ) { a = c; s = p / 4.0; }
    else { s = p / ( 6.2831853072 ) * asin( c / a ); }
    if( t < 1.0 ) return -0.5 * ( a * pow( 2.0, 10.0 * ( t -= 1.0 ) ) * sin( ( t * d - s ) * ( 6.2831853072 ) / p ) ) + b;
    return a * pow( 2.0, -10.0 * ( t -= 1.0 ) ) * sin( ( t * d - s ) * ( 6.2831853072 ) / p ) * 0.5 + c + b;
}

float inOutElastic( float t )
{
    float s = 1.70158; float p = 0; float a = 1.0;
    if( t == 0 ) return 0.0;
    if( ( t /= 0.5 ) == 2.0 ) return 1.0;
    p = ( 0.3 * 1.5 );
    s = p / ( 6.2831853072 ) * asin( 1.0 / a );
    if( t < 1.0 ) return -0.5 * ( a * pow( 2.0, 10.0 * ( t -= 1.0 ) ) * sin( ( t - s ) * ( 6.2831853072 ) / p ) );
    return a * pow( 2.0, -10.0 * ( t -= 1.0 ) ) * sin( ( t - s ) * ( 6.2831853072 ) / p ) * 0.5 + 1.0;
}

float inBack( float t, float b, float c, float d )
{
    float s = 1.70158;
    return c * ( t /= d ) * t * ( ( s + 1.0 ) * t - s ) + b;
}

float inBack( float t )
{
    float s = 1.70158;
    return t * t * ( ( s + 1.0 ) * t - s);
}

float outBack( float t,  float b,  float c,  float d )
{
    float s = 1.70158;
    return c * ( ( t = t / d - 1.0 ) * t * ( ( s + 1.0 ) * t + s ) + 1.0 ) + b;
}

float outBack( float t )
{
    float s = 1.70158;
    return ( ( --t ) * t * ( ( s + 1.0 ) * t + s ) + 1.0);
}

float inOutBack( float t, float b, float c, float d )
{
    float s = 1.70158;
    if( ( t /= d / 2.0 ) < 1.0 ) return c / 2.0 * ( t * t * ( ( ( s *= 1.525 ) + 1.0 ) * t - s ) ) + b;
    return c / 2.0 * ( ( t -= 2.0 ) * t * ( ( ( s *= ( 1.525 ) ) + 1.0 ) * t + s ) + 2.0 ) + b;
}

float inOutBack( float t )
{
    float s = 1.70158;
    if( ( t /= 0.5 ) < 1.0 ) return 0.5 * ( t * t * ( ( ( s *= 1.525 ) + 1.0 ) * t - s ) );
    return 0.5 * ( ( t -= 2 ) * t * ( ( ( s *= ( 1.525 ) ) + 1.0 ) * t + s ) + 2.0 );
}

float outBounce( float t, float b, float c, float d )
{
    if( ( t /= d ) < ( 1.0 / 2.75 ) ) {
        return c * ( 7.5625 * t * t ) + b;
    } else if ( t < ( 2.0 / 2.75 ) ) {
        return c * ( 7.5625 * ( t -= ( 1.5 / 2.75 ) ) * t + 0.75 ) + b;
    } else if ( t < ( 2.5 / 2.75 ) ) {
        return c * ( 7.5625 * ( t -= ( 2.25 / 2.75 ) ) * t + 0.9375 ) + b;
    } else {
        return c * ( 7.5625 * ( t -= ( 2.625 / 2.75 ) ) * t + 0.984375 ) + b;
    }
}

float outBounce( float t )
{
    if( t < ( 1.0 / 2.75 ) ) {
        return ( 7.5625 * t * t );
    } else if ( t < ( 2.0 / 2.75 ) ) {
        return ( 7.5625 * ( t-= ( 1.5 / 2.75 ) ) * t + .75 );
    } else if ( t < ( 2.5 / 2.75 ) ) {
        return ( 7.5625 * ( t -= ( 2.25 / 2.75 ) ) * t + 0.9375 );
    } else {
        return ( 7.5625 * ( t -= ( 2.625 / 2.75 ) ) * t + 0.984375 );
    }
}

float inBounce( float t, float b, float c, float d )
{
    return c - outBounce( d - t, 0.0, c, d ) + b;
}

float inBounce( float t )
{
    return 1.0 - outBounce( 1.0 - t);
}

float inOutBounce( float t, float b, float c, float d )
{
    if ( t < d /2.0 ) return inBounce ( t * 2.0, 0.0, c, d ) * 0.5 + b;
    return outBounce ( t * 2.0 - d, 0, c, d ) * 0.5 + c * 0.5 + b;
}

float inOutBounce( float t )
{
    if ( t < 0.5 ) return inBounce( t * 2.0 ) * 0.5;
    return outBounce( t * 2.0 - 1.0 ) * 0.5 + 0.5;
}
