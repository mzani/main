
/*################################################################*/
/*
		Defines
 */
/*################################################################*/

/*
		%% Gerais %%
*/
// Defines gerais.
#define TAM_MSG_TEXTO							50
#define ANG_GIRO									10
#define TEMPO_ESP_COMANDO					120

// Defines de Verificação.
#define TRUE											1
#define FALSE											0

// Defines de endereços.
#define END_ROBOT									0xf0
#define END_I_O										0xe0
#define END_INT										0xc0

// Defines de velocidades.
#define VEL_GIRO_ANT_HOR					1
#define VEL_GIRO_HOR							-2
#define VEL_DESVIO								2
#define VEL_CONT									5

//	Valores limites para Níveis.
#define MAX_NIVEL									0x05 /* OUT */
#define NIVEL_INICIAL							1

//	Valores limites para Velocidade.
#define VEL_MAX										10
#define VEL_MIN										-10

//	Valores limites para Distância.
#define DIST_MAX									20000
#define DIST_MIN									-20000

//	Valores limites para Graus.
#define GRAU_MAX									20000
#define GRAU_MIN									-20000

//	Valores limites para Tempo Limite.
#define TEMPO_LIM_MAX             10
#define TEMPO_LIM_MIN             1

//	Valores limites para Sensor.
#define VALOR_MAX_SENSOR					4 
#define VALOR_MIN_SENSOR					1
#define FORMATO_SENSOR						1
#define NUM_SENSORES              4 

//	Valores de giro.
#define TEMPO_GIRO_LIMITE         3000
#define TEMPO_GIRO_OBST_FRENTE    5000
#define TEMPO_GIRO_OBST_DIR       5000
#define TEMPO_GIRO_OBST_ESQ       5000
#define TEMPO_LIMITE              4000
#define TEMPO_GIRO_INTELIGENTE		2000
#define TEMPO_GIRO_INICIAL		    3500

// Defines de coordenadas.
#define COORD_X_MAX								500
#define COORD_X_MIN								0
#define COORD_Y_MAX								500
#define COORD_Y_MIN								0



/*
		%% Leds %%
*/
#define END_LEDS									0xe4 
#define END_0_LEDS_LIGADOS				0xff // 1 Led Aceso
#define END_1_LED_LIGADO					0xfe // 1 Led Aceso
#define END_2_LEDS_LIGADOS				0xfc // 2 Leds Acesos
#define END_3_LEDS_LIGADOS				0xf8 // 3 Leds Acesos
#define END_4_LEDS_LIGADOS				0xf0 // 4 Leds Acesos
#define END_5_LEDS_LIGADOS				0xe0 // 5 Leds Acesos
#define END_6_LEDS_LIGADOS				0xc0 // 6 Leds Acesos
#define END_7_LEDS_LIGADOS				0x80 // 7 Leds Acesos
#define END_8_LEDS_LIGADOS				0x00 // 8 Leds Acesos
#define VALOR_ESCALA							19



/*
		%% Display %%
*/
#define END_DSPL									0xe0
#define END_DSPL_BUSY							0xe1
#define END_DSPL_WRITE						0xe2
#define END_DSPL_READ							0xe3
#define END_DSPL_TAM_LINHA				16
#define END_DSPL_CONFIG						0x3c
#define END_DSPL_DESLIGA					0x08
#define END_DSPL_LIMPA						0x01
#define END_DSPL_INC							0x06
#define END_DSPL_LIGA							0x0f
#define END_DSPL_PRIM_POS_L_1			0x80
#define END_DSPL_PRIM_POS_L_2			0xc0
#define END_FLG_BUSY							0x80
#define TEMPO_ESP_INIC						100



/*
		%% Teclas %%
*/
#define END_TECLAS								0xe8
#define VALOR_TECLA_1							0x01
#define VALOR_TECLA_2							0x02
#define VALOR_TECLA_3							0x04
#define VALOR_TECLA_4							0x08
#define VALOR_TECLA_5							0x10
#define VALOR_TECLA_6							0x20
#define VALOR_TECLA_7							0x40
#define VALOR_TECLA_8							0x80
#define TECLA_1										0x01
#define TECLA_2										0x02
#define TECLA_3										0x03
#define TECLA_4										0x04
#define TECLA_5										0x05
#define TECLA_6										0x06
#define TECLA_7										0x07
#define TECLA_8										0x08
#define TEMPO_BOUNCE              6



/*
		%% Movimentação %%
*/

/*	Acrescentar Modos no final e em sequencia	*/
#define START_NIVEL								0x00 /* OUT */
#define MOVE_MM										0x01 /* OUT */
#define MOVE_SPD									0x02 /* OUT */
#define TURN_DEG									0x03 /* OUT */
#define TURN_SPD									0x04 /* OUT */
#define R_WHEEL_SPD								0x05 /* OUT */
#define L_WHEEL_SPD								0x06 /* OUT */
#define STOP											0x07 /* OUT */
#define MAIN_AUTO									0x08 /* OUT */
#define TESTE_SENSORES						0x09 /* OUT */
#define RESET_MODO								0x10 /* OUT */



/*
		%% Serial IR %%
*/
#define END_ON_SIR								0x10 /* OUT */
#define END_OFF_SIR								0x11 /* OUT */
#define END_WRITE_DATA						0x12 /* OUT */
#define END_READ_DATA							0x13 /* IN */
#define END_DATA_OUT_READY				0x14 /* IN */
#define END_DATA_IS_IN						0x15 /* IN */



/*
		%% Sensor IR %%
*/
#define END_ON_GP2D								0x20 /* OUT */
#define END_OFF_GP2D							0x21 /* OUT */
#define END_RD_GP2D_1							0x22 /* IN */
#define END_RD_GP2D_2							0x23 /* IN */
#define END_RD_GP2D_3							0x24 /* IN */
#define END_RD_GP2D_4							0x25 /* IN */
#define END_RD_GP2D_ALL						0x26 /* IN */
#define SENSOR_1									0x01
#define SENSOR_2									0x02
#define SENSOR_3									0x03
#define SENSOR_4									0x04
#define ESPERA_SENSOR             10 
#define NUM_BITS_SENSOR						7
#define NUM_BITS_ESCALA						3
#define ERRO_DISTANCIA						10
#define VALOR_SENSOR_MINIMO				32



/*
		%% Encoder %%
*/
#define END_RD_ENC_R_4B						0x30 /* IN */
#define END_RD_ENC_L_4B						0x31 /* IN */
#define END_RD_ENC_R_2B						0x32 /* IN */
#define END_RD_ENC_L_2B						0x33 /* IN */
#define END_RD_ENC_R_1B						0x34 /* IN */
#define END_RD_ENC_L_1B						0x35 /* IN */
#define END_RESET_ENC_R						0x36 /* OUT */
#define END_RESET_ENC_L						0x37 /* OUT */
#define END_RESET_ENC_ALL					0x38 /* OUT */
#define ESPERA_ENCODER            20



/*
		%% Som %%
*/
#define END_ON_BUZZ								0x40 /* OUT */
#define END_OFF_BUZZ							0x41 /* OUT */
#define END_BIP										0x42 /* IN */



/*
		%% Timer %%
*/
#define END_TMR_ON								0x50 /* OUT */
#define END_TMR_OFF								0x51 /* OUT */
#define END_TMR_MS								0x52 /* OUT */
#define TEMPO_INTERRUPCAO					50 /*milissegundos*/
#define END_TIMER_TESTE_CONT_BIN	0xe7
#define END_TIMER_TESTE_BYTE			0xe5
