#ifndef _SIM_COMMON_H_
#define _SIM_COMMON_H_

extern physical_model model;
extern GLuint texName[10];

#define VERBOSE_CAMERA 0 // print lots of random stuff if 1
#define DELAY 500 // max refresh rate

/* makes texture from given file, outputs textrue name as Gluint */
void makeTextureImage(char filename[], GLuint tex_name);
/* setup camera */
void set_camera();
/* simulates speed */
void anim_scene(); 
/* reads textures and define lighting */
void init();
/* destroys defined objects */
void destroy();



#endif
