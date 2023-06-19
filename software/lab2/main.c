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
#define TOP_LIMIT 0
#define LEFT_LIMIT 0
#define RIGHT_LIMIT 310
#define BOTTOM_LIMIT 210
typedef struct Cursor{
	int x;
	int y;
} Cursor;

int main(void)
{
	//printf("debut main\n\r");
	alt_putstr("debut du main \n\r");
	// Variables pour souris PS2
	unsigned char left_btn = 0;
	unsigned char right_btn = 0;
	int x_mov = 0;
	int y_mov = 0;
	int x_pos = 0;
	int y_pos = 0;
	char pos_msg[12];

	Cursor currentCursor;
	Cursor oldCursor;

	/* CHAR BUFFER setup and static display */
	alt_up_char_buffer_dev *char_buffer;
	char_buffer = alt_up_char_buffer_open_dev("/dev/character_buffer_with_dma");
	if(char_buffer == NULL){
		printf("char buff is dead\n\r");
	}else{
		printf("char buff ok\n\r");
	}
	alt_up_char_buffer_init(char_buffer);

	/* PIXEL BUFFER setup and background display */
	alt_up_pixel_buffer_dma_dev *pixel_buffer;
	pixel_buffer=alt_up_pixel_buffer_dma_open_dev(PIXEL_BUFFER_DMA_0_NAME);
	if(pixel_buffer == NULL){
		alt_putstr("pixel buff is dead \n\r");
	}else{
		alt_putstr("pixel buff ok\n\r");
	}
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer,0);
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 0,0,320,240,0xEB,0);

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
                //printf("NiosDraw 1.42.69 - NicolasGagnier - Robin Galipeau");
				printf("X: %d Y: %d\n\r",x_pos,y_pos);
			}

            /* Manage cursor */
			cursorX = x_pos/10
			cursorY = y_pos/10


			/* process clicks */
			if (left_btn){
				
			}else if(right_btn){

			}

			// send new position to char buff
			sprintf(pos_msg, "NiosDraw 1.42.69 - NicolasGagnier - Robin Galipeau");
			alt_up_char_buffer_string(char_buffer, pos_msg, 0,5);
            sprintf(pos_msg, "X:%3d Y:%3d", x_pos, y_pos);
            alt_up_char_buffer_string(char_buffer, pos_msg, 200,200);

			// vertical refresh
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
		}
	}

	return 0;
}