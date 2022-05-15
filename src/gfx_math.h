#ifndef GFX_MATH_H
#define GFX_MATH_H
#pragma warning(disable:4201)

#define PI 3.14159265359f
#define DEG_TO_RAD(D) ((D)*0.0174532925f)
#define RAD_TO_DEG(R) ((R)*57.2957795f)
#define RADIANS(D)    DEG_TO_RAD((D))
#define DEGREES(R)    RAD_TO_DEG((R))
#define SQUARE(X)     ((X)*(X))

typedef union { struct { float x,y; }; struct { float u,v; }; } vec2;
typedef union { struct { float x,y,z; }; struct { float r,g,b; }; } vec3;
typedef union { struct { float x,y,z,w; }; struct { float r,g,b,a; }; } vec4;
/*  Column-major */
typedef float mat3[9];
typedef float mat4[16];

/*
  Note: mat4 is a typedef'd float[16], usage would be the same:

  mat4 transformation = {0};
  transformation[0] = 1.0f;
  etc.

  When passing into functions, these are equivalent:
  float a[16];
  mat4 b;
  init_m4(a, 1.0f);
  init_m4(b, 1.0f);
  
*/


#define MAT(M,ROW,COL) ((M).elements[(ROW)][(COL)])
#define MAT_P(M,ROW,COL) ((M)->elements[(ROW)][(COL)])

void init_v2(vec2* out, float x, float y);
void copy_v2(vec2* out, vec2* target);
int compare_v2(vec2* target, vec2* source);
void add_v2(vec2* out, vec2* a, vec2* b);
void sub_v2(vec2* out, vec2* a, vec2* b);
void scale_v2(vec2* out, float scalar);
float dot_v2(vec2* a, vec2* b);
float length_v2(vec2* a);

void init_v3(vec3* out, float x, float y, float z);
void copy_v3(vec3* out, vec3* target);
int compare_v3(vec3* target, vec3* source);
void add_v3(vec3* out, vec3* a, vec3* b);
void sub_v3(vec3* out, vec3* a, vec3* b);
void scale_v3(vec3* out, float scalar);
float dot_v3(vec3* a, vec3* b);
void cross_v3(vec3* out, vec3* a, vec3* b);
float length_v3(vec3* a);

void init_v4(vec4* out, float x, float y, float z, float w);
void copy_v4(vec4* out, vec4* target);
int compare_v4(vec4* target, vec4* source);
void add_v4(vec4* out, vec4* a, vec4* b);
void sub_v4(vec4* out, vec4* a, vec4* b);
void scale_v4(vec4* out, float scalar);
float dot_v4(vec4* a, vec4* b);
float length_v4(vec4* a);

/*
  3x3 MATRIX FUNCTIONS
*/

void init_m3(mat3 out, float scalar);
void init_diag_m3(mat3 out, float scalar);
void copy_m3(mat3 out, mat3 target);
void mult_m3(mat3 out, mat3 a, mat3 b);
void scale_m3(mat3 out, float x, float y); /* Last row untouched */
void scale_whole_m3(mat3 out, float scalar);

/* Scale individual row */
void scale_x_m3(mat3 source, float scalar);
void scale_y_m3(mat3 source, float scalar);
void scale_z_m3(mat3 source, float scalar);

/* Transforms */
void m3_mult_v3(vec3* out, mat3 mat, vec3* v);
void translate_m3(mat3 out, vec2* vec);

/*
  4x4 MATRIX FUNCTIONS
*/

void init_m4(mat4 out, float scalar);
void init_diag_m4(mat4 out, float scalar);
void copy_m4(mat4 out, mat4 target);
void mult_m4(mat4 out, mat4 a, mat4 b);
void scale_m4(mat4 out, float x, float y, float z); /* Last row untouched */
void scale_whole_m4(mat4 out, float scalar);

void m4_mult_v4(vec4* out, mat4 mat, vec4* v);

/* Transforms */
void translate_m4(mat4 out, vec3* vec);
/* Supply a rotation matrix an angle theta and then multiply it with the input matrix */
void rotate_m4(mat4 out, float theta, vec3* axis);

#endif

#ifdef GFX_MATH_IMPL

#ifdef GFX_GL

#include <math.h>
#define SQRT_F(N) (float)sqrt(N)

/*
  
  VECTOR 2
  
*/

void init_v2(vec2* out, float x, float y)
{
    out->x = x;
    out->y = y;
}

void copy_v2(vec2* a, vec2* out)
{
    out->x = a->x;
    out->y = a->y;
}

int is_same_v2(vec2* a, vec2* b)
{
    return (
        (a->x == b->x)
        && (a->y == b->y));
}

void add_v2(vec2* out, vec2* a, vec2* b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}

void sub_v2(vec2* out, vec2* a, vec2* b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
}

void scale_v2(vec2* a, float scalar)
{
    a->x *= scalar;
    a->y *= scalar;
}

float dot_v2(vec2* a, vec2* b)
{
    return (a->x * b->x) + (a->y * b->y);
}

float length_v2(vec2* a)
{
    return SQRT_F((a->x * a->x) + (a->y * a->y));
}

/*
  
  VECTOR 3
  
*/

void init_v3(vec3* out, float x, float y, float z)
{
    out->x = x;
    out->y = y;
    out->z = z;
}

void copy_v3(vec3* out, vec3* a)
{
    out->x = a->x;
    out->y = a->y;
    out->z = a->z;
}

int is_same_v3(vec3* a, vec3* b)
{
    return (
        (a->x == b->x)
        && (a->y == b->y)
        && (a->z == b->z));
}

void add_v3(vec3* out, vec3* a, vec3* b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void sub_v3(vec3* out, vec3* a, vec3* b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

void scale_v3(vec3* a, float scalar)
{
    a->x *= scalar;
    a->y *= scalar;
    a->z *= scalar;
}

float dot_v3(vec3* a, vec3* b)
{
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

void cross_v3(vec3* a, vec3* b, vec3* out)
{
    out->x = a->y*b->z - a->z*b->y;
    out->y = a->z*b->x - a->x*b->z;
    out->z = a->x*b->y - a->y*b->x;    
}

float length_v3(vec3* a)
{
    return SQRT_F((a->x * a->x) + (a->y * a->y) + (a->z * a->z));
}

/*
  
  VECTOR 4
  
*/

void init_v4(vec4* out, float x, float y, float z, float w)
{
    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;
}

void copy_v4(vec4* a, vec4* out)
{
    out->x = a->x;
    out->y = a->y;
    out->z = a->z;
    out->w = a->w;    
}

int is_same_v4(vec4* a, vec4* b)
{
    return (
        (a->x == b->x)
        && (a->y == b->y)
        && (a->z == b->z)
        && (a->w == b->w));
}

void add_v4(vec4* out, vec4* a, vec4* b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
    out->w = a->w + b->w;    
}

void sub_v4(vec4* out, vec4* a, vec4* b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
    out->w = a->w - b->w;    
}

void scale_v4(vec4* a, float scalar)
{
    a->x *= scalar;
    a->y *= scalar;
    a->z *= scalar;
    a->w *= scalar;
}

float dot_v4(vec4* a, vec4* b)
{
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z) + (a->w * b->w);;
}

float length_v4(vec4* a)
{
    return SQRT_F((a->x * a->x) + (a->y * a->y) + (a->z * a->z) + (a->w * a->w));
}

/*
  
  MAT3
  
*/

void init_m3(mat3 out, float scalar)
{
    out[0] = scalar;
    out[1] = scalar;
    out[2] = scalar;
    out[3] = scalar;
    out[4] = scalar;
    out[5] = scalar;
    out[6] = scalar;
    out[7] = scalar;
    out[8] = scalar;
    out[9] = scalar;
}

void init_diag_m3(mat3 out, float scalar)
{
    out[0]  = scalar;
    out[1]  = 0.0f;
    out[2]  = 0.0f;
    out[3]  = 0.0f;
    out[4]  = scalar;
    out[5]  = 0.0f;
    out[6]  = 0.0f;
    out[7]  = 0.0f;
    out[8]  = scalar;  
}    

void copy_m3(mat3 out, mat3 source)
{
    out[0]  = source[0];
    out[1]  = source[1];
    out[2]  = source[2];
    out[3]  = source[3];
    out[4]  = source[4];
    out[5]  = source[5];
    out[6]  = source[6];
    out[7]  = source[7];
    out[8]  = source[8];
    out[9]  = source[9];
}

void mult_m3(mat3 out, mat3 a, mat3 b)
{
    
    // First row
    out[0] =  a[0] * b[0]      +  a[3] * b[1]      + a[6] * b[2];
    out[3] =  a[0] * b[3]      +  a[3] * b[4]      + a[6] * b[5];
    out[6] =  a[0] * b[6]      +  a[3] * b[7]      + a[6] * b[8];

    // Second row                                                
    out[1] =  a[1] * b[0]      +  a[4] * b[1]      + a[7] * b[2];
    out[4] =  a[1] * b[3]      +  a[4] * b[4]      + a[7] * b[5];
    out[7] =  a[1] * b[6]      +  a[4] * b[7]      + a[7] * b[8];
    
    // Third row
    out[2] =  a[2] * b[0]      +  a[5] * b[1]      + a[8] * b[2];
    out[5] =  a[2] * b[3]      +  a[5] * b[4]      + a[8] * b[5];
    out[8] =  a[2] * b[6]      +  a[5] * b[7]      + a[8] * b[8];
}

// Used on 'standard' matrices used for 2D operations
void scale_m3(mat3 out, float x, float y)
{
    out[0] *= x;    
    out[4] *= y;          
}

void scale_whole_m3(mat3 source, float scalar)
{
    source[0] *= scalar;
    source[1] *= scalar;
    source[2] *= scalar;
    source[3] *= scalar;
    source[4] *= scalar;
    source[5] *= scalar;
    source[6] *= scalar;
    source[7] *= scalar;
    source[8] *= scalar;      
}

void m3_mult_v3(vec3* out, mat3 a, vec3* v)
{
    out->x = a[0]  * v->x      + a[3]  * v->y     + a[6]  * v->z;
    out->y = a[1]  * v->x      + a[4]  * v->y     + a[7]  * v->z;
    out->z = a[2]  * v->x      + a[5]  * v->y     + a[8] * v->z;
}

void translate_m3(mat3 out, vec2* translation)
{
    out[6] = translation->x;
    out[7] = translation->y;    
}

/*
  
  MAT4
  
*/

void init_m4(mat4 out, float scalar)
{
    out[0] = scalar;
    out[1] = scalar;
    out[2] = scalar;
    out[3] = scalar;
    out[4] = scalar;
    out[5] = scalar;
    out[6] = scalar;
    out[7] = scalar;
    out[8] = scalar;
    out[9] = scalar;
    out[10] = scalar;
    out[11] = scalar;
    out[12] = scalar;
    out[13] = scalar;
    out[14] = scalar;
    out[15] = scalar;    
}

void init_diag_m4(mat4 out, float scalar)
{
    out[0]  = scalar;
    out[1]  = 0.0f;
    out[2]  = 0.0f;
    out[3]  = 0.0f;
    out[4]  = 0.0f;
    out[5]  = scalar;
    out[6]  = 0.0f;
    out[7]  = 0.0f;
    out[8]  = 0.0f;
    out[9]  = 0.0f;
    out[10] = scalar;
    out[11] = 0.0f;
    out[12] = 0.0f;
    out[13] = 0.0f;
    out[14] = 0.0f;
    out[15] = scalar;    
}    

void copy_m4(mat4 out, mat4 source)
{
    out[0]  = source[0];
    out[1]  = source[1];
    out[2]  = source[2];
    out[3]  = source[3];
    out[4]  = source[4];
    out[5]  = source[5];
    out[6]  = source[6];
    out[7]  = source[7];
    out[8]  = source[8];
    out[9]  = source[9];
    out[10] = source[10];
    out[11] = source[11];
    out[12] = source[12];
    out[13] = source[13];
    out[14] = source[14];
    out[15] = source[15];
}

void mult_m4(mat4 out, mat4 a, mat4 b)
{
    
    /* First row */
    out[0] =  a[0] * b[0]      +  a[4] * b[1]      + a[8] * b[2]       + a[12] * b[3];
    out[4] =  a[0] * b[4]      +  a[4] * b[5]      + a[8] * b[6]       + a[12] * b[7];
    out[8] =  a[0] * b[8]      +  a[4] * b[9]      + a[8] * b[10]      + a[12] * b[11];
    out[12] =  a[0] * b[12]     +  a[4] * b[13]     + a[8] * b[14]      + a[12] * b[15];

    /* Second row */
    out[1] =  a[1] * b[0]      +  a[5] * b[1]      + a[9] * b[2]       + a[13] * b[3];
    out[5] =  a[1] * b[4]      +  a[5] * b[5]      + a[9] * b[6]       + a[13] * b[7];
    out[9] =  a[1] * b[8]      +  a[5] * b[9]      + a[9] * b[10]      + a[13] * b[11];
    out[13] =  a[1] * b[12]     +  a[5] * b[13]     + a[9] * b[14]      + a[13] * b[15];
    
    /* Third row */
    out[2] =  a[2] * b[0]      +  a[6] * b[1]      + a[10] * b[2]       + a[14] * b[3];
    out[6] =  a[2] * b[4]      +  a[6] * b[5]      + a[10] * b[6]       + a[14] * b[7];
    out[10] = a[2] * b[8]      +  a[6] * b[9]      + a[10] * b[10]      + a[14] * b[11];
    out[14] = a[2] * b[12]     +  a[6] * b[13]     + a[10] * b[14]      + a[14] * b[15]; 

    /* Fourth row */
    out[3] = a[3] * b[0]      +  a[7] * b[1]      + a[11] * b[2]       + a[15] * b[3];
    out[7] = a[3] * b[4]      +  a[7] * b[5]      + a[11] * b[6]       + a[15] * b[7];
    out[11] = a[3] * b[8]      +  a[7] * b[9]      + a[11] * b[10]      + a[15] * b[11];
    out[15] = a[3] * b[12]     +  a[7] * b[13]     + a[11] * b[14]      + a[15] * b[15]; 
}

/* Used on 'standard' matrices used for 3D operations meaning it will only be used on the diagonal except for the last element */
void scale_m4(mat4 out, float x, float y, float z)
{
    out[0] *= x;    
    out[5] *= y;   
    out[0] *= z;        
}

void scale_whole_m4(mat4 source, float scalar)
{
    source[0] *= scalar;
    source[1] *= scalar;
    source[2] *= scalar;
    source[3] *= scalar;
    source[4] *= scalar;
    source[5] *= scalar;
    source[6] *= scalar;
    source[7] *= scalar;
    source[8] *= scalar;
    source[9] *= scalar;
    source[10] *= scalar;
    source[11] *= scalar;
    source[12] *= scalar;
    source[13] *= scalar;
    source[14] *= scalar;
    source[15] *= scalar;       
}

void m4_mult_v4(vec4* out, mat4 a, vec4* v)
{
    out->x = a[0] * v->x      + a[4] * v->y     + a[8]  * v->z     + a[12] * v->w;
    out->y = a[1] * v->x      + a[5] * v->y     + a[9]  * v->z     + a[13] * v->w;
    out->w = a[2] * v->x      + a[6] * v->y     + a[10] * v->z     + a[14] * v->w;
    out->z = a[3] * v->x       + a[7] * v->y     + a[11] * v->z     + a[15] * v->w;        
}

void translate_m4(mat4 out, vec3* translation)
{
    out[12] = translation->x;
    out[13] = translation->y;
    out[14] = translation->z;    
}

void rotate_m4(mat4 out, float theta, vec3* axis)
{    
    mat4 temp, rot;
    
    rot[0]  = (float)cos(theta) + SQUARE(axis->x)*(1-(float)cos(theta));
    rot[1]  = axis->y*axis->x*(1-(float)cos(theta)) + axis->z*(float)sin(theta);
    rot[2]  = axis->z*axis->x*(1-(float)cos(theta)) - axis->y*(float)sin(theta);
    rot[3]  = 0.0f;
    
    rot[4]  = axis->x*axis->y*(1-(float)cos(theta)) - axis->z*(float)sin(theta);
    rot[5]  = (float)cos(theta) + SQUARE(axis->y)*(1-(float)cos(theta));
    rot[6]  = axis->z*axis->y*(1-(float)cos(theta)) + axis->x*(float)sin(theta);
    rot[7]  = 0.0f;

    rot[8]  = axis->x*axis->z*(1-(float)cos(theta)) + axis->y*(float)sin(theta);
    rot[9]  = axis->y*axis->z*(1-(float)cos(theta)) - axis->x*(float)sin(theta);
    rot[10] = (float)cos(theta) + SQUARE(axis->z)*(1-(float)cos(theta));
    rot[11] = 0.0f;

    rot[12] = 0.0f;
    rot[13] = 0.0f;
    rot[14] = 0.0f;
    rot[15] = 1.0f;   

    mult_m4(temp, out, rot);
    copy_m4(out, temp);
    
}

/*
  Very similar to glm::ortho,
   inspired by https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/orthographic-projection-matrix
   http://www.songho.ca/opengl/gl_projectionmatrix.html   
*/

void frustum(mat4 out, float l, float r, float t, float b, float n, float f)
{
    out[0] = 2.0f * n / (r - l);
    out[1] = 0.0f;
    out[2] = 0.0f;
    out[3] = 0.0f;
    out[4] = 0.0f;
    out[5] =  2.0f * n / (t - b);
    out[6] = 0.0f;
    out[7] = 0.0f;
    out[8] = (r + l) / (r - l);
    out[9] = (t + b) / (t - b);    
    out[10] = (n + f) / (n - f);
    out[11] = -1.0f;
    out[12] = 0.0f;
    out[13] = 0.0f;
    out[14] = 2*f*n / (n-f);
    out[15] = 0.0f;
}

void ortho(mat4 out, float l, float r, float b, float t, float n, float f)
{
    out[0] = 2.0f / (r-l);
    out[1] = 0.0f;
    out[2] = 0.0f;
    out[3] = 0.0f;
    out[4] = 0.0f;
    out[5] = 2.0f / (t-b);
    out[6] = 0.0f;
    out[7] = 0.0f;
    out[8] = 0.0f;
    out[9] = 0.0f;    
    out[10] = - 2.0f / (f - n);
    out[11] = 0.0f;
    out[12] = -(r + l) / (r - l);
    out[13] = -(t + b) / (t - b);
    out[14] = -(f + n) / (f - n);
    out[15] = 1.0f;
}

/*
  https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix
*/
void perspective(mat4 out, float fov, float aspect, float n, float f)
{
    float fov_angle = (float)tan(fov / 2.0f);
    out[0] = 1.0f / (aspect * fov_angle);
    out[1] = 0.0f;
    out[2] = 0.0f;
    out[3] = 0.0f;
    out[4] = 0.0f;
    out[5] =  1.0f / fov_angle;
    out[6] = 0.0f;
    out[7] = 0.0f;
    out[8] = 0.0f;
    out[9] = 0.0f;
    out[10] = (n + f) / (n - f);
    out[11] = -1.0f;
    out[12] = 0.0f;
    out[13] = 0.0f;
    out[14] = 2.0f * f *n / (n-f);
    out[15] = 0.0f;
}

#endif
#endif
