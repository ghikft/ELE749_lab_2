/*
 * main.c
 *
 *  Created on: Jun 12, 2023
 *      Author: ghikf
 */
/* C Library */
#include <stdio.h>		// printf()
#include <string.h> 	// sprintf()

/* Altera-specific */
#include "alt_types.h"
#include "system.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"

/* Mouse driver */
#include "ps2_mouse.h"

//Screen border limts
#define TOP_LIMIT 10
#define LEFT_LIMIT 0
#define RIGHT_LIMIT 319
#define BOTTOM_LIMIT 239
#define SCALE_FACTOR 10
//Colors
#define BACKGROUD_COLOR 128//0xEB
#define DRAW_COLOR 0
#define CURSOR_COLOR 0xFF

typedef struct Cursor{
	int x;
	int y;
} Cursor;

unsigned char get_pixel_color(int x, int y){
	/**************************************************************************
	 * Get the color value of a specific pixel
	 **************************************************************************
	 * Parameters
	 * x : x coordinate of the pixel
	 * y : y coordinate of the pixel
	 *
	 * Valeur de retour
	 * specified pixel color
	 *
	 * Side effects
	 * none
	 *************************************************************************/
	unsigned char *pixel;
	unsigned char color;
	pixel = (unsigned char *)ONCHIP_MEM_BASE+(y<<9)+x;
	color = *pixel;
	return color;
}

int main(void)
{
	//printf("debut main\n\r");
	alt_putstr("debut du main \n\r");
	// Variables pour souris PS2
	unsigned char left_btn = 0;
	unsigned char right_btn = 0;
	int x_mov = 0;
	int y_mov = 0;
	int x_pos = 10;
	int y_pos = 10;
	char pos_msg[18];
	int lastRight = 0;
	int lastLeft = 0;
	int lastColor = 0;

	//Init cursor at the top left of the drawing zone
	Cursor currentCursor;
	currentCursor.x = 0;
	currentCursor.y = 10;

	/* CHAR BUFFER setup and static display */
	alt_up_char_buffer_dev *char_buffer;
	char_buffer = alt_up_char_buffer_open_dev("/dev/character_buffer_with_dma");
	if(char_buffer == NULL){
		printf("char buff is dead\n\r");
	}else{
		printf("char buff ok\n\r");
	}
	alt_up_char_buffer_init(char_buffer);
	sprintf(pos_msg, "NiosDraw 1.42.69 - Nicolas Gagnier - Robin Galipeau");
	alt_up_char_buffer_string(char_buffer, pos_msg, 0,1);

	/* PIXEL BUFFER setup and background display */
	alt_up_pixel_buffer_dma_dev *pixel_buffer;
	pixel_buffer=alt_up_pixel_buffer_dma_open_dev(PIXEL_BUFFER_DMA_0_NAME);
	if(pixel_buffer == NULL){
		alt_putstr("pixel buff is dead \n\r");
	}else{
		alt_putstr("pixel buff ok\n\r");
	}
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer,0);
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 0,0,320,240,BACKGROUD_COLOR,0);

	ps2_init(); 		// from ps2_mouse.h
	printf("init complete\n");
	
	/* main loop */
	while (1) {
		
		// process ps2 events during vertical blank
		if (!alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer)) {

			// process PS2 events
			if (ps2_process(&left_btn, &right_btn, &x_mov, &y_mov)) {
				x_pos += x_mov;
				y_pos -= y_mov;
			}

            /* Manage cursor */
			//erase old cursor
			alt_up_pixel_buffer_dma_draw(pixel_buffer,lastColor,currentCursor.x,currentCursor.y);
			
			//Apply scaling and verify cursor is within the boundarys of the screen
			if (x_pos > RIGHT_LIMIT*SCALE_FACTOR){
				currentCursor.x = RIGHT_LIMIT;
				x_pos = RIGHT_LIMIT*SCALE_FACTOR;
			}else if (x_pos < LEFT_LIMIT*SCALE_FACTOR){
				currentCursor.x = LEFT_LIMIT;
				x_pos = LEFT_LIMIT*SCALE_FACTOR;
			}else{
				currentCursor.x = x_pos/10;
			}
			
			if(y_pos> BOTTOM_LIMIT*SCALE_FACTOR){
				currentCursor.y = BOTTOM_LIMIT;
				y_pos = BOTTOM_LIMIT*SCALE_FACTOR;
			}else if(y_pos < TOP_LIMIT*SCALE_FACTOR){
				currentCursor.y = TOP_LIMIT;
				y_pos = TOP_LIMIT*SCALE_FACTOR;
			}else{
				currentCursor.y = y_pos/SCALE_FACTOR;
			}
			//Save the last cursor pixel color for next turn in the loop
			lastColor = get_pixel_color(currentCursor.x, currentCursor.y);
			//Draw cursor
			alt_up_pixel_buffer_dma_draw(pixel_buffer,CURSOR_COLOR,currentCursor.x,currentCursor.y);

			/* process clicks */
			if (left_btn){ //Draw during left click
				alt_up_pixel_buffer_dma_draw(pixel_buffer,DRAW_COLOR,currentCursor.x,currentCursor.y);
				lastColor = DRAW_COLOR;
				if(!lastLeft){
					alt_putstr("left clik, DRAWING\n\r");
					printf("Drawing at: X:%d Y:%d\n\r",currentCursor.x, currentCursor.y);
				}else{
					printf("Drawing at: X:%d Y:%d\n\r",currentCursor.x, currentCursor.y);
				}
				lastLeft = 1;
			}else if(right_btn){ //erase whole screen if right click
				alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 0,0,320,240,BACKGROUD_COLOR,0);
				if(!lastRight){
					printf("right click (X:%d,Y:%d), CLEAR SCREEN\n\r",currentCursor.x, currentCursor.y);
				}
				lastRight = 1;
			}
			else{ //clear click flags
				if(lastLeft){
					alt_putstr("left released, STOP DRAWING\n\r");
				}
				lastLeft = 0;
				lastRight = 0;
			}

			// send new position to char buff
            sprintf(pos_msg, "X:%d Y:%d  ", currentCursor.x, currentCursor.y);
            alt_up_char_buffer_string(char_buffer, pos_msg, 60,59);

			// vertical refresh
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
		}
	}
	return 0;
}
