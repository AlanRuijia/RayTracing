#include "sphere.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#define DEBUG_MODE_1 1
using namespace std;

extern int ChessBoard_on;
extern Vector BoardEdgeNorm[4] ;
extern Point BoardPoints[4];
extern float BoardLength;
extern float ChessBoardCoefficient[4];

float precision = 0.001;
void printSPoint(Point  P){
        cout << "Sphere Point: " << P.x << " " << P.y << " " << P.z << endl;
}
/********************************************************************
  Itersect Chess board
 ***********************************************************************/
float intersect_board(Point o, Vector u, Spheres *sph, Point *hit) {

        float t = -(ChessBoardCoefficient[0] * o.x + ChessBoardCoefficient[1] * o.y + ChessBoardCoefficient[2] * o.z + ChessBoardCoefficient[3])
                /(ChessBoardCoefficient[0] * u.x + ChessBoardCoefficient[1] * u.y + ChessBoardCoefficient[2] * u.z);
        Point temp;
        temp.x = o.x + t*u.x;
        temp.y = o.y + t*u.y; 
        temp.z = o.z + t*u.z;
        Vector v;
        float flag; 
        int i;
        for (i=0;i<4;i++){
                v = get_vec(BoardPoints[i],temp);
                flag = vec_dot(v,BoardEdgeNorm[i]);
                if (flag <0)
                        break;
        }
        if (i < 4)
                return -1;
        hit->x = o.x + t*u.x;
        hit->y = o.y + t*u.y; 
        hit->z = o.z + t*u.z;
        return t;

}
/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 **********************************************************************/
float intersect_sphere(Point o, Vector u, Spheres *sph, Point *hit, int choose) {

        Vector EyeToCenter = get_vec(sph->center,o);

        float A,B,C;
        A = vec_dot(u,u);
        B = 2 * vec_dot(u, EyeToCenter);
        C = vec_dot(EyeToCenter,EyeToCenter) - pow(sph->radius,2);
        // A = pow(u.x,2) + pow(u.y,2) + pow(u.z,2);
        // B = 2*(u.x*(sph->center.x - o.x) + u.y*(sph->center.y - o.y) + u.z*(sph->center.z - o.z));
        // C = pow((o.x - sph->center.x),2) + pow((o.y - sph->center.y),2) + pow((o.z - sph->center.z),2) - pow(sph->radius,2);
        float Delta;
        Delta = B*B/4 - A*C;
        // if (Delta>0){
        //     cout << "B" << B << endl;
        //     cout << Delta << endl;
        // }
        if (Delta < -precision)
                return -1.0;
        else if (-precision <= Delta && Delta <= precision){
                float t = -B/2;
                if (t < -precision)
                    return -1.0;
                else {
                    (*hit) = get_point(o,vec_scale(u,t));
                    // cout << "t\n";
                    return t;
                }
            }
            else{
                float t1 = -B/2 + sqrt(Delta);
                float t2 = -B/2 - sqrt(Delta);
                if(t1 < -precision) // both wrong direction
                    return -1.0;
                else if(t2 < -precision) { // only one right direction
                    (*hit) = get_point(o,vec_scale(u,t1));
                    // cout << "t1\n";
                    return t1;
                }
                else{
                    if (choose){
                            (*hit) = get_point(o,vec_scale(u,t2));
                            // cout << "t2\n";
                            return t2;
                    }else{
                            (*hit) = get_point(o,vec_scale(u,t1));
                            // cout << "t1\n";
                            return t1;
                    }
                }
                // float t = -(-B + sqrt(Delta)) / (2 * A);
                // hit->x = o.x + t*u.x;
                // hit->y = o.y + t*u.y; 
                // hit->z = o.z + t*u.z;
                // //  cout << "t: " << t << endl;;
                // // cout << "U: " << u.x << " " << u.y << " " << u.z << endl;
                // //  cout << "HIT: " << (*hit).x <<  " "<<(*hit).y <<  " "<< (*hit).z << endl;
                // return t;
        }
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
 // o eye u ray
Spheres *intersect_scene(Point o, Vector u, Spheres *sph, Point *hit,int ignore) {

        float t;
        bool t_set = false;

        Point NewHit, RetHit;
        // NewHit.x = NewHit.y = NewHit.z = -101;
        // hit->z = -100;
        Spheres * RetSph = NULL;

        while(sph!=NULL){
                float temp;
                if (sph->index == ignore){
                    sph = sph->next;
                    continue;
                }
                if (sph->index==0&&ChessBoard_on == 1){
                        temp = intersect_board(o, u, sph, &NewHit);
                }
                else {
                        if (ChessBoard_on==0 && sph->index == 0){
                                sph = sph->next;
                                continue;
                        }
                        temp = intersect_sphere(o, u, sph, &NewHit,1);
                }

                if (temp > precision){
                    if (!t_set || temp < t){
                        t_set = true;
                        t = temp;
                        RetHit = NewHit;
                        RetSph = sph;
                    }
                }
                // if (temp < t && temp != -1){
                //         t = temp;
                //         hit->x = NewHit.x;
                //         hit->y = NewHit.y;
                //         hit->z = NewHit.z;
                //         RetSph = sph;
                // }
                sph = sph -> next;
        }

        if (t_set){
            (*hit) = RetHit; 
        }
        return RetSph;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Spheres *add_sphere(Spheres *slist, Point ctr, float rad, float amb[],
                float dif[], float spe[], float shine, 
                float refl, int sindex, bool is_Refracted,float refractivity,float refractance) {
        Spheres *new_sphere;

        new_sphere = (Spheres *)malloc(sizeof(Spheres));
        new_sphere->index = sindex;
        new_sphere->center = ctr;
        new_sphere->radius = rad;
        (new_sphere->mat_ambient)[0] = amb[0];
        (new_sphere->mat_ambient)[1] = amb[1];
        (new_sphere->mat_ambient)[2] = amb[2];
        (new_sphere->mat_diffuse)[0] = dif[0];
        (new_sphere->mat_diffuse)[1] = dif[1];
        (new_sphere->mat_diffuse)[2] = dif[2];
        (new_sphere->mat_specular)[0] = spe[0];
        (new_sphere->mat_specular)[1] = spe[1];
        (new_sphere->mat_specular)[2] = spe[2];
        new_sphere->mat_shineness = shine;
        new_sphere->reflectance = refl;
        new_sphere->next = NULL;
        new_sphere->is_Refracted = is_Refracted;
        new_sphere->refractivity = refractivity;
        new_sphere -> refractance = refractance;

        if (slist == NULL) { // first object
                slist = new_sphere;
        } else { // insert at the beginning
                new_sphere->next = slist;
                slist = new_sphere;
        }

        return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
Vector sphere_normal(Point q, Spheres *sph) {
        Vector rc;

        rc = get_vec(sph->center, q);
        normalize(&rc);
        return rc;
}
