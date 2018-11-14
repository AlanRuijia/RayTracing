### Ray Tracing
*******************************                             				     		                        				    
*    Author: Ruijia(Alan) Mao    		 
*
*    E-mail: rma61@sfu.ca     
*                                   				
*    StudentID: 301295769         		                             				     		                           				
*******************************
This project implements a ray tracer in C++.

### Usage 

- Use -u (user) to render a customized scene.
  - User scene is defined by set_up_user_scene() in scene.cpp
- Use -d to render the default scene.
  - Default scene is defined by set_up_default_scene() in scene.cpp
- The second argument specifies the number of recursion levels. 
  - Set step_max = 0 to have no reflected or refracted ray cast. 
- The remaining arguments are used to switch on following features. 
  * +s: inclusion of shadows;
  * +l: inclusion of reflection;
  * +r: inclusion of refraction;
  * +c: inclusion of chess board pattern;
  * +f: enabling diffuse rendering through stochastic ray generations;
  * +p: enabling super-sampling; 
