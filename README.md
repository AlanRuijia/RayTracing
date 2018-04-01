*******************************
*                                   				
*    README For Ray Tracing         		
*                                   				    
*    Created by Ruijia(Alan) Mao    		 
*                                   				
*    Student Num: 301295769         		
*                                   				
*    E-mail : rma61@sfu.ca          		
*                                   				
*                                   				
*******************************

This is a ray tracer.

### Usage 

The option –u (user) should be used if you want to render a scene you created 
(to be defined in the function set_up_user_scene() in scene.cpp). 

To render the default scene, defined in the function set_up_default_scene() in scene.cpp, use –d. 
The next argument specifies the number of recursion levels. 
For example, when step_max = 0, then no reflected or refracted rays are to be cast. 
The remaining arguments can be used to turn on different features of your raytracer. 
The default, if no flag is provided, is to turn the feature off.

There are a total of six feature:

* +s: inclusion of shadows
* +l: inclusion of reflection
* +r: inclusion of refraction
* +c: inclusion of chess board pattern
* +f: enabling diffuse rendering using stochastic ray generations
* +p: enabling super-sampling 
