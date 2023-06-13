/****** ps2_mouse.c *********************************************************
*Description
* Librairie pour controler la souris par le port PS/2
*********************************************************************
*Auteurs : Michal Solecki, Patrick Salois, utilis� avec permission
*Creation date : 2014-07-07
*Last Modified : 2014-07-14
*********************************************************************/

#include <io.h>
#include <alt_types.h>
#include <system.h>
#include <stdio.h>
#include <altera_up_avalon_ps2.h>
#include <altera_up_ps2_mouse.h>
#include "sys/alt_irq.h"
#include "ps2_mouse.h"

//Variables globales
context_t ps2_context;
message_queue_t message_q;
alt_up_ps2_dev *ps2;

/**************************************************************************
* Fonction d'interruption du traitement des donnees du port PS/2
**************************************************************************
*Parametres
* contexte:		Contient les variables pour communiquer avec le
* 				programme principal
*
*Valeur de retour
* aucune
*
*Notes
* Aucune
*
*************************************************************************/
#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
static void ps2_isr (void *context) {
#else
static void ps2_isr (void *context, alt_u32 id) {
#endif

	static unsigned char ps2_state = PS2_IDLE;	//Variable du statut de la machine a etat
	static unsigned char ps2_align_flag = 0;	//Variable qui indique que les donnees
												//ne sont pas alignees
	context_t *ctxt  = context;					//Structure qui contient le contexte pour
												//communiquer avec l'interruption
	unsigned char new_data;						//Donnee entrante

	if (alt_up_ps2_read_data_byte(ctxt->ps2, &new_data) == 0) {

		switch (ps2_state) {
		case PS2_IDLE:
		WR_MSG(ctxt->message_q, 0) = new_data;		// lecture du 1er byte
			ps2_state = PS2_DATA_1;					//prochain etat
			//Le premier byte devrait avec un 1 au 3e bit(0x08)
			if ((new_data & PS2_ALIGN_MASK) == 0)	ps2_align_flag = 1;
			break;

		case PS2_DATA_1:
			if (new_data == 0x00 && WR_MSG(ctxt->message_q, 0) == 0xAA) {
				//seqeunce de RESET detectee (0xAA00)
				ctxt->reset_flag = 1;					//drapeau pour demander un reset
				ps2_state = PS2_IDLE;					//prochain etat
			}
			else {
				WR_MSG(ctxt->message_q, 1) = new_data;	//lecture du 2e byte
				ps2_state = PS2_DATA_2;					//prochain etat

				if (ps2_align_flag  && (new_data & PS2_ALIGN_MASK)) {
					//si un alignement des bytes est necessaire
					WR_MSG(ctxt->message_q, 0) = new_data;	//realignement des donnees
					ps2_state = PS2_DATA_1;			//nouvel etat apres alignement
					ps2_align_flag = 0;				//On desactive le drapeau d'alignement
				}
			}

			break;
		case PS2_DATA_2:
			if (new_data == 0x00 && WR_MSG(ctxt->message_q, 1) == 0xAA){
				//seqeunce de RESET detectee (0xAA00)
				ctxt->reset_flag = 1;				//drapeau pour demander un reset
				ps2_state = PS2_IDLE;				//prochain etat
			}
			else {
				if (ps2_align_flag  && (new_data & PS2_ALIGN_MASK)) {
					//si un alignement des bytes est necessaire
					WR_MSG(ctxt->message_q, 0) = new_data;
					ps2_state = PS2_DATA_1;			//nouvel etat apres alignement
					ps2_align_flag = 0;				//On desactive le drapeau d'alignement
				}
				else {
					WR_MSG(ctxt->message_q, 2) = new_data;	//lecture du 3e byte
					ctxt->message_q->write_addr++;			//pointeur de l'adresse d'ecriture
					if (ctxt->message_q->write_addr == MSG_Q_SIZE)
						ctxt->message_q->write_addr = 0;
				}
			}
			ps2_state = PS2_IDLE;							//prochain etat
			break;

		default:
			ps2_state = PS2_IDLE;							//prochain etat
			break;
		}
	}
}

void ps2_init(void) {
	ps2 = alt_up_ps2_open_dev(PS2_0_NAME);		//Connexion au module PS/2

	// On initialise les variables dans le contexte
	message_q.read_addr = 0;	//addresse de lecture dans le buffer circulaire
	message_q.write_addr = 0;	//addresse d'ecriture dans le buffer circulaire

	ps2_context.ps2 = ps2;		//pointeur vers le module PS/2
	ps2_context.reset_flag = 0;	//marqueur de demande de RESET
	ps2_context.ready = 0;		//marqueur pour indiquer que le port est pret
	ps2_context.message_q = &message_q;	//pointeur vers le buffer circulaire de messages


	// On enregistre la fonction qui traite l'interruption
	#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
	alt_ic_isr_register(	PS2_IRQ_INTERRUPT_CONTROLLER_ID,
							PS2_IRQ, ps2_isr,
							(void *) (&ps2_context),
							0x0 );
	#else
	alt_irq_register(PS2_0_IRQ, (void *) (&ps2_context), ps2_isr);
	#endif

	alt_up_ps2_enable_read_interrupt(ps2); //On active les interruptions
}

long ps2_process(	unsigned char *left_btn,
					unsigned char *right_btn,
					int *x_mov,
					int *y_mov)
{
	long res;
	unsigned char status;

	alt_up_ps2_disable_read_interrupt(ps2); // On desactive les interruptions
											// durant la lecture

	if (ps2_context.reset_flag) {
		//Drapeau de demande RESET detecte
		alt_up_ps2_mouse_reset(ps2);	//commande RESET (0xFF) envoye au PS2
		ps2_context.reset_flag = 0;		//Drapeau de demande de RESET
		ps2_context.ready = 1;			//Drapeau du statut du module PS/2
		res = 0;
	}
	else
	{
		res = 1;
	}

	if (res) res = (message_q.read_addr != message_q.write_addr);

	if (res) {
		//On lit la derniere valeur des boutons
		status = RD_MSG((&message_q), 0);

		// On verifie le bouton gauche
		*left_btn = (LEFT_BUT == (status & LEFT_BUT));

		// On verifie le bouton droit
		*right_btn = (RIGHT_BUT == (status & RIGHT_BUT));

		//On verifie le deplacement sur l'axe des x
		*x_mov = RD_MSG((&message_q), 1);
		if (status & MOUSE_X_SIGN) *x_mov -= 256;

		//On verifie le deplacement sur l'axe des y
		*y_mov = RD_MSG((&message_q), 2);
		if (status & MOUSE_Y_SIGN) *y_mov -= 256;

		//On avance le pointeur de lecture dans le buffer circulaire
		message_q.read_addr++;
		if(message_q.read_addr == MSG_Q_SIZE) message_q.read_addr = 0;
	}

	alt_up_ps2_enable_read_interrupt(ps2); //On reactive les interruptions

	return res;
}
