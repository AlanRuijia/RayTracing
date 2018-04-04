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
This project implements a ray tracer.

### Usage 

- Use -u (user) to render a user customized scene.
  - User scene is defined in set_up_user_scene() in scene.cpp
- use -d to render the default scene
  - Default scene is defined in set_up_default_scene() in scene.cpp
- The second argument specifies the number of recursion levels. 
  - When step_max = 0, then no reflected or refracted rays are to be cast. 
- The remaining arguments can be used to turn on different features. 
  * +s: inclusion of shadows
  * +l: inclusion of reflection
  * +r: inclusion of refraction
  * +c: inclusion of chess board pattern
  * +f: enabling diffuse rendering using stochastic ray generations
  * +p: enabling super-sampling 
