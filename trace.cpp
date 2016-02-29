#include "include/mat.h"
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include "global.h"
#include "sphere.h"
#include <fstream>

#define DEBUG1 0

using namespace std;
//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern Spheres *scene;

// light 1 position and color
extern Point light1;
extern float light1_ambient[3];
extern float light1_diffuse[3];
extern float light1_specular[3];
//Black ChessBoard
Vector BoardNorm;
float BlackChessBoard_ambient[3] = {0,0,0};
float BlackChessBoard_diffuse[3] = {0,0,0};
float BlackChessBoard_specular[3] = {0,0,0};
// global ambient term
extern float global_ambient[3];

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int Refractance_on;
extern int ChessBoard_on;
extern int AntiAliase_on;
extern int Reflection_on;
extern int InterDiffuse;

extern float precision;

extern int step_max;
extern Vector BoardEdgeNorm[4] ;
extern Point BoardPoints[4];
extern float BoardLength;
extern float ChessBoardCoefficient[4];
float BoardBottom[2];
float BoardUnit;
/////////////////////////////////////////////////////////////////////
extern RGB_float recursive_ray_trace(Point eye, Vector ray, Spheres *sph,int times,int);
void printPoint(Point  P){
        cout << "Sphere Point: " << P.x << " " << P.y << " " << P.z << endl;
}
Vector get_reflected_ray(Vector ray, Vector surf_norm){
    ray = vec_scale(ray,-1);
    Vector temp = vec_minus(vec_scale(surf_norm,2*vec_dot(surf_norm,ray)) ,ray);
    normalize(&temp);

    return temp;
}

float random(float a, float b){

    return ( (float) rand() / RAND_MAX * (b-a) + a );
}

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
RGB_float phong(Point q, Vector v, Vector surf_norm, Spheres *sph) {
        //q : hit
        // v: ray
        //
        Spheres * shadow;
        Vector ShadowRay;
        Point ShadowHit;
        bool is_shadowed = false;
        Vector L = get_vec(q,light1);
        float d = vec_len(L);
        normalize(&L);
        float f = 1/(decay_a + decay_b*d + decay_c * pow(d,2));
        float fatt = f < 1? f:1;
        Vector R;
        float BD,BS,temp;

        //Diffuse Reflection
        BD = vec_dot(surf_norm,L)>0?vec_dot(surf_norm,L):0;

        //Reflected Ray
        R =   vec_minus(vec_scale(surf_norm,2* vec_dot(surf_norm,L)) ,L);
        normalize(&R);
        

        //Specular Reflection
        temp = vec_dot(R,v) > 0 ? vec_dot(R,v) : 0;
        BS = pow(temp,sph->mat_shineness);

        RGB_float color,other,ambient;
        ambient.r = global_ambient[0] * sph -> mat_ambient[0] + light1_ambient[0] * sph -> mat_ambient[0];
        ambient.g = global_ambient[1] * sph -> mat_ambient[1] + light1_ambient[1] * sph -> mat_ambient[1];
        ambient.b = global_ambient[2] * sph -> mat_ambient[2] + light1_ambient[2] * sph -> mat_ambient[2];

        other.r =  fatt * (light1_diffuse[0]*sph->mat_diffuse[0] *BD  + light1_specular[0] * sph -> mat_specular[0]* BS) ;
        other.g =  fatt * (light1_diffuse[1]*sph->mat_diffuse[1] *BD  + light1_specular[1] * sph -> mat_specular[1]* BS) ;
        other.b =  fatt * (light1_diffuse[2]*sph->mat_diffuse[2] *BD  + light1_specular[2] * sph -> mat_specular[2]* BS) ;

        color.r = ambient.r  + other.r;
        color.g = ambient.g  + other.g;
        color.b = ambient.b  + other.b;

         if (ChessBoard_on && sph->index == 0){
                int row,col;
                row = (int)((q.x- BoardBottom[0])/BoardUnit)%2;
                col = (int)((q.z- BoardBottom[1])/BoardUnit)%2;
                if ((!row & col)|(row & !col)){
                        color.r =  global_ambient[0] * BlackChessBoard_ambient[0] + light1_ambient[0] * BlackChessBoard_ambient[0]  + fatt * (light1_diffuse[0]* BlackChessBoard_diffuse[0] *BD  + light1_specular[0] * BlackChessBoard_specular[0]* BS) ;
                        color.g =  global_ambient[1] * BlackChessBoard_ambient[1] + light1_ambient[1] * BlackChessBoard_ambient[1]  + fatt * (light1_diffuse[1]* BlackChessBoard_diffuse[1] *BD  + light1_specular[1] * BlackChessBoard_specular[1]* BS) ;
                        color.b =  global_ambient[2] * BlackChessBoard_ambient[2] + light1_ambient[2] * BlackChessBoard_ambient[2]  + fatt * (light1_diffuse[2]* BlackChessBoard_diffuse[2] *BD  + light1_specular[2] * BlackChessBoard_specular[2]* BS) ;
                }
        }

        other.r =  color.r - ambient.r;
        other.g =  color.g - ambient.g;
        other.b =  color.b - ambient.b;

        if (shadow_on==1){
                ShadowRay = get_vec(q,light1);
                if (intersect_scene(q,ShadowRay,scene,&ShadowHit,sph->index)!=NULL){
                    is_shadowed = true;
                }
                else {
                        float dis = vec_len(get_vec(ShadowHit,q));
                        if (dis < precision){
                            is_shadowed = true;
                        }
                    }

                if (is_shadowed){
                         return ambient;

                }
            }

       
        return color;
}

bool GetRefractRay(Vector inVector, Point inPoint, Vector *outVector, Spheres* sph){
    
    Vector Norm = get_vec(sph->center , inPoint);
    inVector = vec_scale(inVector,-1);
    normalize(&inVector);

    float i;
    if(vec_dot(Norm,inVector))
        i = 1.0/sph->refractivity;
    else{
        Norm = vec_scale(Norm,-1);
        i = sph->refractivity;
    }

    float r = 1- i*i *(1 - pow(vec_dot(Norm, inVector),2));

    if (r < precision) return false;

    (*outVector) = vec_minus(vec_scale(Norm, i * vec_dot(Norm,inVector) - sqrt(r)) , vec_scale(inVector,i));
    return true;

}
bool Refract(Vector inVector, Point inPoint, Vector *outVector, Point * outPoint, Spheres* sph){
    Vector MiddleVector, RetVector;
    Point RetPoint;

    if(!GetRefractRay(inVector, inPoint,&MiddleVector,sph))
        return false;

    float Distance = intersect_sphere(inPoint, MiddleVector, sph,&RetPoint,0);
    if (Distance < precision) 
        return false;
    if (!GetRefractRay(MiddleVector,RetPoint, &RetVector,sph))
        return false;

    (*outVector) = RetVector;
    (*outPoint) = RetPoint;
    return true;

}
/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Point eye, Vector ray, Spheres *sph,int times,int ignore) {

        Vector SurfNormal;
        Point hit;
        Spheres* flag;
        RGB_float white,red,green,gray;
        white.r = white.g = white.b = 1.0;
        gray.r = gray.g = gray.b = 0.3;
        red.r = 1;
        green.g = 1;
        red.g = red.b = green.r = green.b = 0;
        normalize(&ray);
        flag = intersect_scene(eye,ray,sph,&hit,ignore);

        // if (flag != NULL)
           //  cout << "NOT NULL " << endl;
        if (flag == NULL){
                return background_clr;
        }
        else {
                // printPoint(hit);

                Vector view = get_vec(hit,eye);
                normalize(&view);
                RGB_float Ret_Color;
                SurfNormal = get_vec(flag->center,hit);
                if (flag->index==0)
                    SurfNormal = BoardNorm;
                normalize(&SurfNormal);
                

                Ret_Color = phong(hit,view, SurfNormal,flag);

                if (Reflection_on && times > 0){
                    Vector Reflected_ray = get_reflected_ray(ray,SurfNormal);
                    // if ((vec_dot(Reflected_ray,SurfNormal) == vec_dot(vec_scale(ray,-1),SurfNormal))&& (vec_dot(vec_cross(ray,Reflected_ray),SurfNormal))==0)
                    //     cout << "T\n";
                        Ret_Color = clr_add(Ret_Color,clr_scale(recursive_ray_trace(hit,Reflected_ray,scene,times-1,flag->index),sph->reflectance));
                }

                if (Refractance_on && times>0 && sph->is_Refracted){
                        Vector OutRay;
                        Point OutPoint;
                        if (Refract(ray,hit,&OutRay,&OutPoint,flag))
                            Ret_Color = clr_add(Ret_Color,clr_scale(recursive_ray_trace(OutPoint,OutRay,scene,times-1,flag->index),sph->refractance));
                }
                if (InterDiffuse && step_max - times < 2){
                        for (int i = 0;i<DIFFUSE;i++){
                            Vector temp = get_reflected_ray(ray,SurfNormal);
                            vec4 Diffuse_Ray;
                            Diffuse_Ray.x = temp.x;
                            Diffuse_Ray.y = temp.y;
                            Diffuse_Ray.z = temp.z;
                            Diffuse_Ray[3] = 0;
                            mat4 RotateMat;
                            float theta[3] = {random(-1.0,1.0),random(-1.0,1.0),random(-1.0,1.0)};
                            RotateMat = RotateX(theta[0]) * RotateY(theta[1]) * RotateZ(theta[2]);
                            Diffuse_Ray = RotateMat * Diffuse_Ray;
                             Vector d = {Diffuse_Ray.x, Diffuse_Ray.y, Diffuse_Ray.z};

                            Ret_Color = clr_add(Ret_Color,clr_scale(recursive_ray_trace(hit,d,scene,times-1,flag->index),0.1));
                            
                        }
                }
                return Ret_Color;
        }
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
        int i, j;
        float x_grid_size = image_width / float(win_width);
        float y_grid_size = image_height / float(win_height);
        float x_start = -0.5 * image_width;
        float y_start = -0.5 * image_height;
        RGB_float ret_color;
        Point cur_pixel_pos[5];
        Vector ray;

        // ray is cast through center of pixel
        cur_pixel_pos[0].x = x_start + 0.25 * x_grid_size;
        cur_pixel_pos[0].y = y_start + 0.25 * y_grid_size;
        cur_pixel_pos[0].z = image_plane;
        cur_pixel_pos[1].z = image_plane;
        cur_pixel_pos[2].z = image_plane;
        cur_pixel_pos[3].z = image_plane;
        cur_pixel_pos[4].z = image_plane;
        Spheres * Chess = scene;
        if (ChessBoard_on){
                while(Chess!=NULL){
                        if (Chess->index == 0)
                                break;
                        Chess = Chess->next;
                }
                Point Center = Chess->center;
                float Radius = Chess->radius;

                BoardPoints[0].x = Center.x + Radius;
                BoardPoints[0].y = Center.y;
                BoardPoints[0].z = Center.z + Radius;

                BoardPoints[1].x = Center.x + Radius;
                BoardPoints[1].y = Center.y;
                BoardPoints[1].z = Center.z - Radius;

                BoardPoints[2].x = Center.x - Radius;
                BoardPoints[2].y = Center.y ;
                BoardPoints[2].z = Center.z  - Radius;

                BoardPoints[3].x = Center.x - Radius;
                BoardPoints[3].y = Center.y;
                BoardPoints[3].z = Center.z + Radius;

                Vector a1 = get_vec(BoardPoints[1],BoardPoints[0]),
                       a2 = get_vec(BoardPoints[2],BoardPoints[1]),
                       a3 = get_vec(BoardPoints[3],BoardPoints[2]),
                       a4 = get_vec(BoardPoints[0],BoardPoints[3]);

                BoardNorm = vec_cross(a2,a1);
                BoardEdgeNorm[0] = vec_cross(BoardNorm,a1);
                BoardEdgeNorm[1] = vec_cross(BoardNorm,a2);
                BoardEdgeNorm[2] = vec_cross(BoardNorm,a3);
                BoardEdgeNorm[3] = vec_cross(BoardNorm,a4);

                BoardLength = 2*Radius;

                BoardBottom[0] = Center.x - Radius;
                BoardBottom[1] = Center.z - Radius;
                BoardUnit = BoardLength/8;
        }

        srand(time(NULL));

        for (i=0; i<win_height; i++) {
                for (j=0; j<win_width; j++) {
                        ret_color.r = ret_color.g = ret_color.b = 0;
                        if (!AntiAliase_on){
                                ray = get_vec( eye_pos,cur_pixel_pos[0]);
                                ret_color = clr_add(recursive_ray_trace(eye_pos, ray, scene, step_max,-1),ret_color);
                        }else{
                                cur_pixel_pos[1].x = cur_pixel_pos[0].x + 0.25 * x_grid_size;
                                cur_pixel_pos[1].y = cur_pixel_pos[0].y + 0.25 * y_grid_size;

                                cur_pixel_pos[2].x = cur_pixel_pos[0].x + 0.5 * x_grid_size;
                                cur_pixel_pos[2].y = cur_pixel_pos[0].y + 0.5 * y_grid_size;

                                cur_pixel_pos[3].x = cur_pixel_pos[0].x ;
                                cur_pixel_pos[3].y = cur_pixel_pos[0].y + 0.5 * y_grid_size;

                                cur_pixel_pos[4].x = cur_pixel_pos[0].x + 0.5 * x_grid_size;
                                cur_pixel_pos[4].y = cur_pixel_pos[0].y;

                                for (int cc = 0; cc<5 ; cc++){
                                        ray = get_vec( eye_pos, cur_pixel_pos[cc]);
                                        ret_color = clr_add(recursive_ray_trace(eye_pos, ray, scene, step_max,-1),ret_color);

                                }
                                ret_color= clr_scale(ret_color,0.2);
                        }

                        frame[i][j][0] = GLfloat(ret_color.r);
                        frame[i][j][1] = GLfloat(ret_color.g);
                        frame[i][j][2] = GLfloat(ret_color.b);
                        //out  << frame[i][j][0] << " " << frame[i][j][1] << " " << frame[i][j][2] << endl; 
                        cur_pixel_pos[0].x += x_grid_size;
                }

                cur_pixel_pos[0].y += y_grid_size;
                cur_pixel_pos[0].x = x_start;
        }
        
}
