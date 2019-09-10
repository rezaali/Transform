int bayer8x8[64] = int[](
    0, 32, 8, 40, 2, 34, 10, 42,
    48, 16, 56, 24, 50, 18, 58, 26,
    12, 44, 4, 36, 14, 46, 6, 38,
    60, 28, 52, 20, 62, 30, 54, 22,
    3, 35, 11, 43, 1, 33, 9, 41,
    51, 19, 59, 27, 49, 17, 57, 25,
    15, 47, 7, 39, 13, 45, 5, 37,
    63, 31, 55, 23, 61, 29, 53, 21
);

float dither8x8(vec2 pos) {
    int px = int(pos.x) % 8;
    int py = int(pos.y) % 8;
    return bayer8x8[py * 8 + px];
}

vec3 dither8x8(vec2 pos, vec3 color) {
    return color + (dither8x8(pos) / 32.0 - (1.0 / 128.0)) / 255.0;
}

vec4 dither8x8(vec2 pos, vec4 color) {
    return color + (dither8x8(pos) / 32.0 - (1.0 / 128.0)) / 255.0;
}