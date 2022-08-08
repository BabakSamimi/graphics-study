/*
  Version 0.1.0

*/

#ifndef GFX_MATH_H
#define GFX_MATH_H
#pragma warning(disable:4201)
/*
  TODO:
  * Handle float comparions?
*/
#define PI 3.14159265359f
#define DEG_TO_RAD(D) ((D)*0.0174532925f)
#define RAD_TO_DEG(R) ((R)*57.2957795f)
#define RADIANS(D)    DEG_TO_RAD((D))
#define DEGREES(R)    RAD_TO_DEG((R))
#define SQUARE(X)     ((X)*(X))

typedef union { struct { float x,y; }; struct { float u,v; }; struct { float width, height; };} vec2;
typedef union { struct { float x,y,z; }; struct { float r,g,b; }; } vec3;
typedef union { struct { float x,y,z,w; }; struct { float r,g,b,a; }; } vec4;

/*  Column-major */
typedef struct {
    float matrix[9];
} mat3x3;

typedef struct {
    float matrix[16];
} mat4x4;

//----------------------
// VEC2
//----------------------
vec2 create_vec2    (float x, float y);
vec2 copy_vec2      (vec2 target);

vec2 add_vec2       (vec2 a, vec2 b);
vec2 sub_vec2       (vec2 a, vec2 b);
vec2 negate_vec2    (vec2 a);
vec2 scale_vec2     (vec2 a, float scalar);
vec2 normalize_vec2 (vec2 a);

float dot_vec2        (vec2 a, vec2 b);
float length_vec2     (vec2 a);
int compare_vec2      (vec2 target, vec2 source);

//----------------------
// VEC3
//----------------------
vec3 create_vec3    (float x, float y, float z);
vec3 copy_vec3      (vec3 target);

vec3 add_vec3       (vec3 a, vec3 b);
vec3 sub_vec3       (vec3 a, vec3 b);
vec3 negate_vec3    (vec3 a);
vec3 scale_vec3     (vec3 a, float scalar);
vec3 normalize_vec3 (vec3 a);
vec3 cross_vec3     (vec3 a, vec3 b);

float dot_vec3      (vec3 a, vec3 b);
float length_vec3   (vec3 a);
int compare_vec3    (vec3 a, vec3 b);

//----------------------
// VEC4
//----------------------
vec4 init_vec4      (float x, float y, float z, float w);
vec4 copy_vec4      (vec4 a);

vec4 add_vec4       (vec4 a, vec4 b);
vec4 sub_vec4       (vec4 a, vec4 b);
vec4 scale_vec4     (vec4 a, float scalar);
vec4 normalize_vec4 (vec4 a);

float dot_vec4      (vec4 a, vec4 b);
float length_vec4   (vec4 a);
int compare_vec4    (vec4 a, vec4 b);

//----------------------
// MAT3X3
//----------------------
mat3x3 create_mat3x3        (float scalar);
mat3x3 create_diag_mat3x3   (float scalar);
mat3x3 copy_mat3x3          (mat3x3 a);
mat3x3 mult_mat3x3          (mat3x3 a, mat3x3 b);
mat3x3 scale_mat3x3         (mat3x3 a, float x, float y); // Last row untouched
mat3x3 uniform_scale_mat3x3 (mat3x3 a, float scalar);

// Transforms
vec3 mat3x3_mult_vec3         (mat3x3 mat, vec3 v);
mat3x3 translate_mat3x3       (mat3x3 a, vec2 vec);

//----------------------
// MAT4X4
//----------------------
mat4x4 create_mat4x4        (float scalar);
mat4x4 create_diag_mat4x4   (float scalar);
mat4x4 copy_mat4x4          (mat4x4 target);
mat4x4 mult_mat4x4          (mat4x4 a, mat4x4 b);
mat4x4 scale_mat4x4         (mat4x4 a, float x, float y, float z); // Last row untouched

// Transforms
vec4 mat4x4_mult_vec4(mat4x4 mat, vec4 v);
mat4x4 translate_mat4x4(mat4x4 out, vec3 vec);

// Supply a rotation matrix an angle theta and then multiply it with the input matrix
mat4x4 rotate_mat4x4(mat4x4 out, float theta, vec3 axis); // Should we modify the parameter or return a new matrix?


//----------------------
// MISC. FUNCTIONS
//----------------------
mat4x4 frustum(float l,float r, float t, float b,
               float n, float f);

mat4x4 ortho(float l, float r, float b, float t,
             float n, float f);
mat4x4 perspective(float fov, float aspect,
                   float n, float f);

mat4x4 look_at(vec3 cam_pos, vec3 at, vec3 up);

#endif

#ifdef GFX_MATH_IMPL
#undef GFX_MATH_IMPL

#include <math.h>
#define SQRT_F(N) (float)sqrt(N)

//----------------------
// VEC2
//----------------------
vec2 create_vec2(float x, float y)
{
    vec2 result;
    result.x = x;
    result.y = y;
    
    return result;
}

vec2 copy_vec2(vec2 a)
{
    vec2 result;
    result.x = a.x;
    result.y = a.y;
    
    return result;
}

vec2 add_vec2(vec2 a, vec2 b)
{
    vec2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

vec2 sub_vec2(vec2 a, vec2 b)
{
    vec2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}


vec2 negate_vec2(vec2 a)
{
    vec2 result;
    result.x = -a.x;
    result.y = -a.y;

    return result;
}

vec2 scale_vec2(vec2 a, float scalar)
{
    vec2 result;
    result.x = a.x * scalar;
    result.y = a.y * scalar;

    return result;
}

vec2 normalize_vec2(vec2 a)
{
    vec2 result;
    
    float length = length_vec2(a);
    
    result.x = a.x / length;
    result.y = a.y / length;
    
    return result;
}

float dot_vec2(vec2 a, vec2 b)
{
    return (a.x * b.x) + (a.y * b.y);
}

float length_vec2(vec2 a)
{
    return SQRT_F(SQUARE(a.x) + SQUARE(a.y));
}

int compare_vec2(vec2 a, vec2 b)
{
    return (
        (a.x == b.x)
        && (a.y == b.y));
}


//----------------------
// VEC3
//----------------------
vec3 create_vec3(float x, float y, float z)
{
    vec3 result;
    
    result.x = x;
    result.y = y;
    result.z = z;

    return result;
}

vec3 copy_vec3(vec3 a)
{
    vec3 result;
    
    result.x = a.x;
    result.y = a.y;
    result.z = a.z;

    return result;
}

vec3 add_vec3(vec3 a, vec3 b)
{
    vec3 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

vec3 sub_vec3(vec3 a, vec3 b)
{
    vec3 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    
    return result;
}

vec3 negate_vec3(vec3 a)
{
    vec3 result;
    
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;

    return result;
}

vec3 scale_vec3(vec3 a, float scalar)
{
    vec3 result;
    
    result.x = a.x * scalar;
    result.y = a.y * scalar;
    result.z = a.z * scalar;

    return result;
}

vec3 normalize_vec3(vec3 a)
{
    vec3 result;
    
    float length = length_vec3(a);
    
    result.x = a.x / length;
    result.y = a.y / length;
    result.z = a.z / length;

    return result;
}

vec3 cross_vec3(vec3 a, vec3 b)
{
    vec3 result;
    
    result.x = (a.y * b.z) - (a.z * b.y);
    result.y = (a.z * b.x) - (a.x * b.z);
    result.z = (a.x * b.y) - (a.y * b.x);

    return result;
}

float dot_vec3(vec3 a, vec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float length_vec3(vec3 a)
{
    return SQRT_F(SQUARE(a.x) + SQUARE(a.y) + SQUARE(a.z));
}

int compare_vec3(vec3 a, vec3 b)
{
    return (
        (a.x == b.x)
        && (a.y == b.y)
        && (a.z == b.z));
}

//----------------------
// VEC4
//----------------------
vec4 init_vec4(float x, float y, float z, float w)
{
    vec4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;

    return result;
}

vec4 copy_vec4(vec4 a)
{
    vec4 result;
    result.x = a.x;
    result.y = a.y;
    result.z = a.z;
    result.w = a.w;

    return result;
}

vec4 add_vec4(vec4 a, vec4 b)
{
    vec4 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    
    return result;
}

vec4 sub_vec4(vec4 a, vec4 b)
{
    vec4 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;

    return result;
}

vec4 scale_vec4(vec4 a, float scalar)
{
    vec4 result;
    
    result.x = a.x * scalar;
    result.y = a.y * scalar;
    result.z = a.z * scalar;
    result.w = a.w * scalar;
    
    return result;
}

vec4 normalize_vec4(vec4 a)
{
    vec4 result;
    
    float length = length_vec4(a);
    
    result.x = a.x * length;
    result.y = a.y * length;
    result.z = a.z * length;
    result.w = a.w * length;

    return result;
}
float dot_vec4(vec4 a, vec4 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

float length_vec4(vec4 a)
{
    return SQRT_F(SQUARE(a.x) + SQUARE(a.y) + SQUARE(a.z) + SQUARE(a.w));
}

int compare_vec4(vec4 a, vec4 b)
{
    return (
        (a.x == b.x)
        && (a.y == b.y)
        && (a.z == b.z)
        && (a.w == b.w));
}

//----------------------
// MAT3X3
//----------------------
mat3x3 create_m3x3(float scalar)
{
    mat3x3 result;
    
    result.matrix[0] = scalar;
    result.matrix[1] = scalar;
    result.matrix[2] = scalar;
    result.matrix[3] = scalar;
    result.matrix[4] = scalar;
    result.matrix[5] = scalar;
    result.matrix[6] = scalar;
    result.matrix[7] = scalar;
    result.matrix[8] = scalar;
    
    return result;
}

mat3x3 create_diag_m3x3(float scalar)
{
    mat3x3 result;
    
    result.matrix[0]  = scalar;
    result.matrix[1]  = 0.0f;
    result.matrix[2]  = 0.0f;
    result.matrix[3]  = 0.0f;
    result.matrix[4]  = scalar;
    result.matrix[5]  = 0.0f;
    result.matrix[6]  = 0.0f;
    result.matrix[7]  = 0.0f;
    result.matrix[8]  = scalar;

    return result;
}    

mat3x3 copy_m3(mat3x3 a)
{
    mat3x3 result;
    
    result.matrix[0]  = a.matrix[0];
    result.matrix[1]  = a.matrix[1];
    result.matrix[2]  = a.matrix[2];
    result.matrix[3]  = a.matrix[3];
    result.matrix[4]  = a.matrix[4];
    result.matrix[5]  = a.matrix[5];
    result.matrix[6]  = a.matrix[6];
    result.matrix[7]  = a.matrix[7];
    result.matrix[8]  = a.matrix[8];

    return result;
}

mat3x3 mult_m3x3(mat3x3 a, mat3x3 b)
{
    mat3x3 result;
    
    // First row
    result.matrix[0] =  a.matrix[0] * b.matrix[0]    +  a.matrix[3] * b.matrix[1]      + a.matrix[6] * b.matrix[2];
    result.matrix[3] =  a.matrix[0] * b.matrix[3]    +  a.matrix[3] * b.matrix[4]      + a.matrix[6] * b.matrix[5];
    result.matrix[6] =  a.matrix[0] * b.matrix[6]    +  a.matrix[3] * b.matrix[7]      + a.matrix[6] * b.matrix[8];

    // Second row                                                
    result.matrix[1] =  a.matrix[1] * b.matrix[0]    +  a.matrix[4] * b.matrix[1]      + a.matrix[7] * b.matrix[2];
    result.matrix[4] =  a.matrix[1] * b.matrix[3]    +  a.matrix[4] * b.matrix[4]      + a.matrix[7] * b.matrix[5];
    result.matrix[7] =  a.matrix[1] * b.matrix[6]    +  a.matrix[4] * b.matrix[7]      + a.matrix[7] * b.matrix[8];
    
    // Third row
    result.matrix[2] =  a.matrix[2] * b.matrix[0]    +  a.matrix[5] * b.matrix[1]      + a.matrix[8] * b.matrix[2];
    result.matrix[5] =  a.matrix[2] * b.matrix[3]    +  a.matrix[5] * b.matrix[4]      + a.matrix[8] * b.matrix[5];
    result.matrix[8] =  a.matrix[2] * b.matrix[6]    +  a.matrix[5] * b.matrix[7]      + a.matrix[8] * b.matrix[8];

    return result;
}

// Used on 'standard' matrices used for 2D operations
mat3x3 scale_m3x3(mat3x3 a, float x, float y)
{
    mat3x3 result = a;
    
    result.matrix[0] *= x;    
    result.matrix[4] *= y;

    return result;
}

vec3 mat3x3_mult_vec3(mat3x3 a, vec3 v)
{
    vec3 result;
    
    result.x = a.matrix[0]  * v.x     + a.matrix[3]  * v.y     + a.matrix[6]  * v.z;
    result.y = a.matrix[1]  * v.x     + a.matrix[4]  * v.y     + a.matrix[7]  * v.z;
    result.z = a.matrix[2]  * v.x     + a.matrix[5]  * v.y     + a.matrix[8]  * v.z;
    
    return result;
}

mat3x3 translate_mat3x3(mat3x3 a, vec2 translation)
{
    mat3x3 result = a;
    
    result.matrix[6] = translation.x;
    result.matrix[7] = translation.y;

    return result;
}

//----------------------
// MAT4X4
//----------------------
mat4x4 create_mat4x4(float scalar)
{
    mat4x4 result;
    
    result.matrix[0] = scalar;
    result.matrix[1] = scalar;
    result.matrix[2] = scalar;
    result.matrix[3] = scalar;
    result.matrix[4] = scalar;
    result.matrix[5] = scalar;
    result.matrix[6] = scalar;
    result.matrix[7] = scalar;
    result.matrix[8] = scalar;
    result.matrix[9] = scalar;
    result.matrix[10] = scalar;
    result.matrix[11] = scalar;
    result.matrix[12] = scalar;
    result.matrix[13] = scalar;
    result.matrix[14] = scalar;
    result.matrix[15] = scalar;
    
    return result;
}

mat4x4 create_diag_mat4x4(float scalar)
{
    mat4x4 result = {0};
    
    result.matrix[0]  = scalar;
    result.matrix[5]  = scalar;
    result.matrix[10] = scalar;
    result.matrix[15] = scalar;

    return result;
}    

mat4x4 copy_mat4x4(mat4x4 source)
{
    mat4x4 result;
    
    result.matrix[0]  = source.matrix[0];
    result.matrix[1]  = source.matrix[1];
    result.matrix[2]  = source.matrix[2];
    result.matrix[3]  = source.matrix[3];
    result.matrix[4]  = source.matrix[4];
    result.matrix[5]  = source.matrix[5];
    result.matrix[6]  = source.matrix[6];
    result.matrix[7]  = source.matrix[7];
    result.matrix[8]  = source.matrix[8];
    result.matrix[9]  = source.matrix[9];
    result.matrix[10] = source.matrix[10];
    result.matrix[11] = source.matrix[11];
    result.matrix[12] = source.matrix[12];
    result.matrix[13] = source.matrix[13];
    result.matrix[14] = source.matrix[14];
    result.matrix[15] = source.matrix[15];

    return result;
}

mat4x4 mult_mat4x4(mat4x4 a, mat4x4 b)
{
    mat4x4 result;
    
    /* First row */
    result.matrix[0] =  a.matrix[0] * b.matrix[0]      +  a.matrix[4] * b.matrix[1]      + a.matrix[8] * b.matrix[2]       + a.matrix[12] * b.matrix[3];
    result.matrix[4] =  a.matrix[0] * b.matrix[4]      +  a.matrix[4] * b.matrix[5]      + a.matrix[8] * b.matrix[6]       + a.matrix[12] * b.matrix[7];
    result.matrix[8] =  a.matrix[0] * b.matrix[8]      +  a.matrix[4] * b.matrix[9]      + a.matrix[8] * b.matrix[10]      + a.matrix[12] * b.matrix[11];
    result.matrix[12] =  a.matrix[0] * b.matrix[12]     +  a.matrix[4] * b.matrix[13]     + a.matrix[8] * b.matrix[14]      + a.matrix[12] * b.matrix[15];

    /* Second row */
    result.matrix[1] =  a.matrix[1] * b.matrix[0]      +  a.matrix[5] * b.matrix[1]      + a.matrix[9] * b.matrix[2]       + a.matrix[13] * b.matrix[3];
    result.matrix[5] =  a.matrix[1] * b.matrix[4]      +  a.matrix[5] * b.matrix[5]      + a.matrix[9] * b.matrix[6]       + a.matrix[13] * b.matrix[7];
    result.matrix[9] =  a.matrix[1] * b.matrix[8]      +  a.matrix[5] * b.matrix[9]      + a.matrix[9] * b.matrix[10]      + a.matrix[13] * b.matrix[11];
    result.matrix[13] =  a.matrix[1] * b.matrix[12]     +  a.matrix[5] * b.matrix[13]     + a.matrix[9] * b.matrix[14]      + a.matrix[13] * b.matrix[15];
    
    /* Third row */
    result.matrix[2] =  a.matrix[2] * b.matrix[0]      +  a.matrix[6] * b.matrix[1]      + a.matrix[10] * b.matrix[2]       + a.matrix[14] * b.matrix[3];
    result.matrix[6] =  a.matrix[2] * b.matrix[4]      +  a.matrix[6] * b.matrix[5]      + a.matrix[10] * b.matrix[6]       + a.matrix[14] * b.matrix[7];
    result.matrix[10] = a.matrix[2] * b.matrix[8]      +  a.matrix[6] * b.matrix[9]      + a.matrix[10] * b.matrix[10]      + a.matrix[14] * b.matrix[11];
    result.matrix[14] = a.matrix[2] * b.matrix[12]     +  a.matrix[6] * b.matrix[13]     + a.matrix[10] * b.matrix[14]      + a.matrix[14] * b.matrix[15]; 

    /* Fourth row */
    result.matrix[3] = a.matrix[3] * b.matrix[0]      +  a.matrix[7] * b.matrix[1]      + a.matrix[11] * b.matrix[2]       + a.matrix[15] * b.matrix[3];
    result.matrix[7] = a.matrix[3] * b.matrix[4]      +  a.matrix[7] * b.matrix[5]      + a.matrix[11] * b.matrix[6]       + a.matrix[15] * b.matrix[7];
    result.matrix[11] = a.matrix[3] * b.matrix[8]      +  a.matrix[7] * b.matrix[9]      + a.matrix[11] * b.matrix[10]      + a.matrix[15] * b.matrix[11];
    result.matrix[15] = a.matrix[3] * b.matrix[12]     +  a.matrix[7] * b.matrix[13]     + a.matrix[11] * b.matrix[14]      + a.matrix[15] * b.matrix[15];

    return result;
}

/* Used on 'standard' matrices used for 3D operations meaning it will only be used on the diagonal except for the last element */
mat4x4 scale_mat4x4(mat4x4 a, float x, float y, float z)
{
    mat4x4 result = a;
    
    result.matrix[0] *= x;    
    result.matrix[5] *= y;   
    result.matrix[10] *= z;

    return result;
}

vec4 mat4x4_mult_vec4(mat4x4 a, vec4 v)
{
    vec4 result = {0};
    
    result.x = a.matrix[0] * v.x      + a.matrix[4] * v.y     + a.matrix[8]  * v.z     + a.matrix[12] * v.w;
    result.y = a.matrix[1] * v.x      + a.matrix[5] * v.y     + a.matrix[9]  * v.z     + a.matrix[13] * v.w;
    result.w = a.matrix[2] * v.x      + a.matrix[6] * v.y     + a.matrix[10] * v.z     + a.matrix[14] * v.w;
    result.z = a.matrix[3] * v.x       + a.matrix[7] * v.y     + a.matrix[11] * v.z     + a.matrix[15] * v.w;

    return result;
}

mat4x4 translate_mat4x4(mat4x4 a, vec3 translation)
{
    mat4x4 result = a;
    
    result.matrix[12] = translation.x;
    result.matrix[13] = translation.y;
    result.matrix[14] = translation.z;
    
    return result;
}

mat4x4 rotate_mat4x4(mat4x4 to_be_rotated_matrix, float theta, vec3 axis)
{    
    mat4x4 rot = {0};
    
    rot.matrix[0]  = (float)cos(theta) + SQUARE(axis.x)*(1-(float)cos(theta));
    rot.matrix[1]  = axis.y*axis.x*(1-(float)cos(theta)) + axis.z*(float)sin(theta);
    rot.matrix[2]  = axis.z*axis.x*(1-(float)cos(theta)) - axis.y*(float)sin(theta);
    rot.matrix[3]  = 0.0f;
    
    rot.matrix[4]  = axis.x*axis.y*(1-(float)cos(theta)) - axis.z*(float)sin(theta);
    rot.matrix[5]  = (float)cos(theta) + SQUARE(axis.y)*(1-(float)cos(theta));
    rot.matrix[6]  = axis.z*axis.y*(1-(float)cos(theta)) + axis.x*(float)sin(theta);
    rot.matrix[7]  = 0.0f;

    rot.matrix[8]  = axis.x*axis.z*(1-(float)cos(theta)) + axis.y*(float)sin(theta);
    rot.matrix[9]  = axis.y*axis.z*(1-(float)cos(theta)) - axis.x*(float)sin(theta);
    rot.matrix[10] = (float)cos(theta) + SQUARE(axis.z)*(1-(float)cos(theta));
    rot.matrix[11] = 0.0f;

    rot.matrix[12] = 0.0f;
    rot.matrix[13] = 0.0f;
    rot.matrix[14] = 0.0f;
    rot.matrix[15] = 1.0f;   

    return mult_mat4x4(rot, to_be_rotated_matrix);
    
}

/*
  Very similar to glm::ortho,
  inspired by:
  https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/orthographic-projection-matrix
  http://www.songho.ca/opengl/gl_projectionmatrix.html   
*/

mat4x4 frustum(float l, float r, float t, float b, float n, float f)
{
    mat4x4 result;
    
    result.matrix[0] = 2.0f * n / (r - l);
    result.matrix[1] = 0.0f;
    result.matrix[2] = 0.0f;
    result.matrix[3] = 0.0f;
    result.matrix[4] = 0.0f;
    result.matrix[5] =  2.0f * n / (t - b);
    result.matrix[6] = 0.0f;
    result.matrix[7] = 0.0f;
    result.matrix[8] = (r + l) / (r - l);
    result.matrix[9] = (t + b) / (t - b);    
    result.matrix[10] = (n + f) / (n - f);
    result.matrix[11] = -1.0f;
    result.matrix[12] = 0.0f;
    result.matrix[13] = 0.0f;
    result.matrix[14] = 2*f*n / (n-f);
    result.matrix[15] = 0.0f;

    return result;
}

mat4x4 ortho(float l, float r, float b, float t, float n, float f)
{
    mat4x4 result;
    
    result.matrix[0] = 2.0f / (r-l);
    result.matrix[1] = 0.0f;
    result.matrix[2] = 0.0f;
    result.matrix[3] = 0.0f;
    result.matrix[4] = 0.0f;
    result.matrix[5] = 2.0f / (t-b);
    result.matrix[6] = 0.0f;
    result.matrix[7] = 0.0f;
    result.matrix[8] = 0.0f;
    result.matrix[9] = 0.0f;
    result.matrix[10] = - 2.0f / (f - n);
    result.matrix[11] = 0.0f;
    result.matrix[12] = -(r + l) / (r - l);
    result.matrix[13] = -(t + b) / (t - b);
    result.matrix[14] = -(f + n) / (f - n);
    result.matrix[15] = 1.0f;

    return result;
}

/*
  https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix
*/
mat4x4 perspective(float fov, float aspect, float n, float f)
{
    mat4x4 result;
    float fov_angle = (float)tan(fov / 2.0f);
    
    result.matrix[0] = 1.0f / (aspect * fov_angle);
    result.matrix[1] = 0.0f;
    result.matrix[2] = 0.0f;
    result.matrix[3] = 0.0f;
    result.matrix[4] = 0.0f;
    result.matrix[5] =  1.0f / fov_angle;
    result.matrix[6] = 0.0f;
    result.matrix[7] = 0.0f;
    result.matrix[8] = 0.0f;
    result.matrix[9] = 0.0f;
    result.matrix[10] = (n + f) / (n - f);
    result.matrix[11] = -1.0f;
    result.matrix[12] = 0.0f;
    result.matrix[13] = 0.0f;
    result.matrix[14] = (2.0f * f * n) / (n - f);
    result.matrix[15] = 0.0f;

    return result;
}

mat4x4 look_at(vec3 cam_pos, vec3 at, vec3 up)
{
    mat4x4 result = {0};
    vec3 x_axis, y_axis, z_axis;
    
    z_axis = sub_vec3(cam_pos, at);    
    z_axis = normalize_vec3(z_axis);
    
    x_axis = cross_vec3(z_axis, up);
    x_axis = normalize_vec3(x_axis);

    y_axis = cross_vec3(x_axis, z_axis);
    
    result.matrix[0] = x_axis.x;
    result.matrix[1] = y_axis.x;
    result.matrix[2] = z_axis.x;
    result.matrix[3] = 0.0f;
    result.matrix[4] = x_axis.y;
    result.matrix[5] = y_axis.y;
    result.matrix[6] = z_axis.y;
    result.matrix[7] = 0.0f;
    result.matrix[8] = x_axis.z;
    result.matrix[9] = y_axis.z;
    result.matrix[10] = z_axis.z;
    result.matrix[11] = 0.0f;    
    result.matrix[12] = -dot_vec3(x_axis, cam_pos);
    result.matrix[13] = -dot_vec3(y_axis, cam_pos);
    result.matrix[14] = -dot_vec3(z_axis, cam_pos);    
    result.matrix[15] = 1.0f;

    return result;
}

#endif
