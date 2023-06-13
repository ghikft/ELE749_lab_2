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
	//if(1){
		//printf("error on creation \n\r");
		alt_putstr("pixel buff is dead \n\r");
	}else{
		//printf("buffer created\n\r");
		alt_putstr("pixel buff ok\n\r");
	}
	//alt_up_pixel_buffer_dma_clear_screen(pixel_buffer,0);
	//usleep(1000000);
	//alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, 0,0,100,100,128,0);
	//alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);

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
				printf("X: %d Y: %d\n\r",x_pos,y_pos);
			}

            /* Manage cursor */


			/* process clicks */


			// send new position to char buff
			sprintf(pos_msg, "X:%3d Y:%3d", x_pos, y_pos);
			alt_up_char_buffer_string(char_buffer, pos_msg, 0,5);

			// vertical refresh
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
		}
	}

	return 0;
}



