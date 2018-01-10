#include <string.h>
#include <stdint.h>

#include "lib/blitter/blitter.h"
#include "lib/sampler/sampler.h"

#include "image_bg.h"
#define TMAP_IMAGE_BG_W 40 // ancho en tiles del background
#define TMAP_IMAGE_BG_H 30 // alto en tiles del background
#define TILE_SIZE 16 // tamanio del tile

//Macros for circle collision
#define Max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
     
#define Min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
     

extern const uint16_t image_bg_tset[]; // share tileset
extern char ninja_spr[];
extern void (*do_frame)(void);

uint16_t image_bg_ramtilemap[TMAP_IMAGE_BG_W][TMAP_IMAGE_BG_H];

/*
 * Global variables
 */
 
int ninjaJump = 0;
int ninjaLeft = 0;
int ninjaRight = 0;
int ninjaFlip = 0;
int ninjaUp = 0;
int ninjaDown = 0;
int collideLeft = 0;
int collideRight = 0;
int collideUp = 0;
int collideDown = 0;
int ninjaSpeed = 1;
int fr_ninja_count = 0;

int start_frame;
int ninja_start_frame; // cuadros a esperar para pasar al siguiente frame de la animación
static struct object *bg, *ninja;

void btbxTemplate_out();

/*
 * Collide Circle with rectangle?
 */
int collideCircle(int circleX,int circleY, int circleRadius, int rectX, int rectY, int rectWidth, int rectHeight)
{    
    int deltaX = circleX - Max(rectX, Min(circleX, rectX + rectWidth));    
    int deltaY = circleY - Max(rectY, Min(circleY, rectY + rectHeight)); 
    return (deltaX * deltaX + deltaY * deltaY) < (circleRadius * circleRadius);
}

/*
 * Move ninja (starring) and check collision with background tiles
 * TODO: upgrade collide system
 */

void moveNinja(){
    int dest_tile;
    if(ninjaLeft) {        
        ninja->x -= 3 * ninjaSpeed;
        
        dest_tile = image_bg_tmap[0][ninja->y/TILE_SIZE * (bg->w / TILE_SIZE) + ninja->x/TILE_SIZE];        
        
        if(dest_tile != 49) 
        {
            ninja->x += 3 * ninjaSpeed;
            collideLeft = 1;            
        }else {        
            dest_tile = image_bg_tmap[0][(ninja->y + ninja->h)/TILE_SIZE * (bg->w / TILE_SIZE) + ninja->x/TILE_SIZE];            
        
            if(dest_tile != 49) 
            {
                ninja->x += 3 * ninjaSpeed;
                collideLeft = 1;                
            }
        }
    }else if(ninjaRight) {        
        ninja->x += 3 * ninjaSpeed;        
        
        dest_tile = image_bg_tmap[0][ninja->y/TILE_SIZE * (bg->w / TILE_SIZE) + (ninja->x+ninja->w)/TILE_SIZE];
        
        if(dest_tile != 49) 
        {
            ninja->x -= 3 * ninjaSpeed;
            collideRight = 1;            
        }else {        
            dest_tile = image_bg_tmap[0][(ninja->y + ninja->h)/TILE_SIZE * (bg->w / TILE_SIZE) + (ninja->x+ninja->w)/TILE_SIZE];
        
            if(dest_tile != 49) 
            {
                ninja->x -= 3 * ninjaSpeed;
                collideRight = 1;               
            }
        }
    }
    
    if(ninjaUp) {                
        ninja->y -= 4;        
        dest_tile = image_bg_tmap[0][ninja->y/TILE_SIZE * (bg->w / TILE_SIZE) + ninja->x/TILE_SIZE];                
        
        if(dest_tile != 49) 
        {
            ninja->y += 4;
            collideUp = 1;            
        }else {
            dest_tile = image_bg_tmap[0][ninja->y/TILE_SIZE * (bg->w / TILE_SIZE) + (ninja->x + ninja->w) /TILE_SIZE];        
            if(dest_tile != 49) 
            {
                ninja->y += 4;
                collideUp = 1;                
            }
        }
        
    }else if(ninjaDown) {               
        ninja->y += 4;        
        dest_tile = image_bg_tmap[0][(ninja->y + ninja->h)/TILE_SIZE * (bg->w / TILE_SIZE) + ninja->x/TILE_SIZE];
        
        if(dest_tile != 49) 
        {         
            ninja->y -= 4;           
            collideDown = 1;                     
        }else {
            dest_tile = image_bg_tmap[0][(ninja->y + ninja->h)/TILE_SIZE * (bg->w / TILE_SIZE) + (ninja->x+ninja->w)/TILE_SIZE];
            if(dest_tile != 49) 
            {                
                ninja->y -= 4;        
                collideUp = 1;                             
            }
        }        
    }
}

/*
 * Test for button pressed
 */

void checkGamepad()
{
    if (GAMEPAD_PRESSED(0,right))
    {
        ninjaRight = 1;
        ninjaLeft = 0;
        ninjaFlip = 0;                  
	}else if (GAMEPAD_PRESSED(0,left)) {
        ninjaRight = 0;
        ninjaLeft = 1;
	    ninjaFlip = 1;             
	}else 
    {
        ninjaRight = 0;
        ninjaLeft = 0;
    }

	if (GAMEPAD_PRESSED(0,up)) {                               
        ninjaUp = 1;			
        ninjaDown = 0;
	}else if (GAMEPAD_PRESSED(0,down)) {
        ninjaUp = 0;
        ninjaDown = 1;                
	}else 
    {
        ninjaUp = 0;
        ninjaDown = 0;
    }
    if (GAMEPAD_PRESSED(0,B)){
        ninjaSpeed = 2;
    }else ninjaSpeed = 1;
    
    if (GAMEPAD_PRESSED(0,start)) btbxTemplate_out();
}

/*
 * Luego de un cuadro actualizamos la lógica
 */

void btbxTemplate_frame()
{  
    if((ninjaRight || ninjaLeft) && (ninja_start_frame > 5))
    {             
        if(ninjaFlip)
        {
            if(fr_ninja_count < 16 && fr_ninja_count > 12) fr_ninja_count++;
            else fr_ninja_count = 13;            
            ninja->fr=fr_ninja_count;
            ninja_start_frame = 0;                
        }
        else
        {
            if(fr_ninja_count < 12 && fr_ninja_count > 8) fr_ninja_count++;
            else fr_ninja_count = 9;            
            ninja->fr=fr_ninja_count;
            ninja_start_frame = 0;
        }
    }else if(ninja_start_frame > 10)
    {            
         if(ninjaFlip)
         {
             if(fr_ninja_count < 3) fr_ninja_count++;
             else fr_ninja_count = 2;
             ninja->fr=fr_ninja_count;            
             ninja_start_frame = 0;
         }else {
             if(fr_ninja_count < 1) fr_ninja_count++;
             else fr_ninja_count = 0;
             ninja->fr=fr_ninja_count;            
             ninja_start_frame = 0;
         }
    }         
    checkGamepad();
    moveNinja();
    ninja_start_frame++;
}

void btbxTemplate_init(int par_1)
{
    bg = tilemap_new(
            image_bg_tset,640,480, 
            TMAP_HEADER(TMAP_IMAGE_BG_W,TMAP_IMAGE_BG_H,TSET_16,TMAP_U8), 
            image_bg_ramtilemap);

	//bg->y = -280; // lo sacamos fuera de la pantalla para testeo del scroll   

    ninja = sprite_new(ninja_spr, 600, 40,0);
    ninja->fr=0;

    start_frame=vga_frame;
	
    tmap_blit(bg,0,0,image_bg_header,image_bg_tmap);

	do_frame = btbxTemplate_frame;
}

void btbxTemplate_out()
{
	// de-allocate objects	
	blitter_remove(bg);
    blitter_remove(ninja);    
}
