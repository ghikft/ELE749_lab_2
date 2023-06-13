
#ifndef PS2_MOUSE_H_
#define PS2_MOUSE_H_

#include <altera_up_avalon_ps2.h>

#define WR_MSG(msg_q, x) msg_q->messages[msg_q->write_addr][x]
#define RD_MSG(msg_q, x) msg_q->messages[msg_q->read_addr][x]
#define MSG_Q_SIZE			10

#define PS2_IDLE			0
#define PS2_DATA_1			1
#define PS2_DATA_2			2
#define PS2_RESET			3

#define LEFT_BUT			1
#define RIGHT_BUT			2
#define MIDDLE_BUT			4

#define MOUSE_X_SIGN		16
#define MOUSE_Y_SIGN		32
#define BUTTON_MASK			0x07
#define PS2_ALIGN_MASK		0x08

/***********************************************************
 *  Definition des structures pour le controle du port PS/2
 ***********************************************************/

// Declaration de la structure du buffer circulaire contenant les messages
typedef struct message_queue {
	volatile unsigned char messages[MSG_Q_SIZE][3];		//Tableau de messages
	volatile unsigned char read_addr;					//Addresse de lecture
	volatile unsigned char write_addr;					//Addresse d'ecriture
}message_queue_t;

//Declaration de la structure du contenant de variables de contexte
typedef struct context {
	volatile unsigned char reset_flag, ready;	//Drapeaux de statut du PS/2
	message_queue_t *message_q;					//Pointeur vers le tableau de message
	alt_up_ps2_dev *ps2;						//Pointeur vers le port PS/2
} context_t;

/***********************************************************
 *  Definition des fonctions
 ***********************************************************/

/**************************************************************************
* Initialise le port et les interruption pour le port PS/2
**************************************************************************
*Parametres
* Aucun
*
*Valeur de retour
* aucune
*
*Notes
* Aucune
*
*************************************************************************/
void ps2_init(void);

/**************************************************************************
* Fonction pour traiter les nouvelles donnees entrantes par le port PS/2
**************************************************************************
*Parametres
* left_btn :	pointeur contenant le statut du bouton gauche
* right_btn :	pointeur contenant le statut du bouton droit
*
* x_mov :		pointeur contenant le deplacement sur l'axe des x
* y_mov :		pointeur contenant le deplacement sur l'axe des y
*
*Valeur de retour
* 1 si une trame a �t� saisie, 0 sinon
*
*Notes
* Aucune
*
*************************************************************************/
long ps2_process(	unsigned char *left_btn,
					unsigned char *right_btn, int *x_mov, int *y_mov);




#endif /* PS2_MOUSE_H_ */
