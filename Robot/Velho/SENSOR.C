/*****************************************************************/
/*      Programa........: Robot																		*/
/*----------------------------------------------------------------*/
/*      Módulo..........: robot.c																	*/
/*----------------------------------------------------------------*/
/*      Início..........: 01/03/2007															*/
/*      Autor...........: Marcelo Tadeu Marchesoni Zani						*/
/*----------------------------------------------------------------*/
/*      Última alteração: 13/03/2007															*/
/*      Responsável.....: Marcelo Tadeu Marchesoni Zani						*/
/*----------------------------------------------------------------*/
/*      01/03/2007, mtmz : Codificação inicial.										*/
/*----------------------------------------------------------------*/
/*      13/03/2007, mtmz : Mudança na estrutura do programa.			*/
/*----------------------------------------------------------------*/
/*      22/03/2007, mtmz : Primeiro teste do programa e função		*/
/*												 que verifica e mostra sensores					*/
/* 												 funcionando.														*/
/******************************************************************/

/*################################################################*/
/*
     Includes
 */
/*################################################################*/

#include "8085io.h"
#include "8085int.h"

#include "C:\USERS\MZani\defines.h"


/*################################################################*/
/*
     Variaveis Globais
 */
/*################################################################*/

int ValorSensor1;
int ValorSensor2;
int ValorSensor3;
int ValorSensor4;
int MostraSensor1;
int MostraSensor2;
int MostraSensor3;
int MostraSensor4;
int TeclaPressionada;
int FinalizaRobo;


/*################################################################*/
/*
     Rotinas de display
 */
/*################################################################*/

/******************************************************************/
/* Função: getStatusLCD																						*/
/******************************************************************/
/* Verifica se o visor está livre*/
int getStatusLCD () 
{
	// Retorna o flag de busy.
	return (in(END_DSPL_BUSY) & END_FLG_BUSY) != 0; 
}


/******************************************************************/
/* Função: setCursorLCD																						*/
/******************************************************************/
/* Posiciona o cursor na posicao c da linha l */
int setCursorLCD (int l, int c) 
{
	int posicao;

	if (l == 1)		
		posicao = END_DSPL_PRIM_POS_L_1;
	else 
		if(l == 2)	
			posicao = END_DSPL_PRIM_POS_L_2;
		else 
			return FALSE; // Se passar linha errada ignora.

	if(c > 40 || c < 1) 
		return FALSE; // Se passar uma posicao invalida, ignora.

	out(END_DSPL, posicao + c - 1);

	return TRUE;
}


/******************************************************************/
/* Função: putChrLCD																							*/
/******************************************************************/
/* Escreve um char no display na posicao atual do cursor */
putChrLCD (char c) 
{
	while(getStatusLCD()); // Aguarda o display desocupar.
	out(END_DSPL_WRITE, c);
}


/******************************************************************/
/* Função: putStrLCD																							*/
/******************************************************************/
/* Escreve uma string no display a partir da posicao atual */
putStrLCD (char *s)
{
	int i;
	for(i=0; s[i] != '\0'; i++)
		putChrLCD (s[i]);
}


/******************************************************************/
/* Função: MostraSensorLed																				*/
/******************************************************************/
/* Escreve os leds no display */
MostraSensorLed(int ValorMedido)
{
	int ValorLed;

	ValorLed = ValorMedido/VALOR_ESCALA;
		// Acende os leds correspondentes
		switch (ValorLed)
		{
				case 0:
					out(END_LEDS, END_1_LED_LIGADO);
					break;

				case 1:
					out(END_LEDS, END_2_LEDS_LIGADOS);
					break;

				case 2:
					out(END_LEDS, END_3_LEDS_LIGADOS);
					break;

				case 3:
					out(END_LEDS, END_4_LEDS_LIGADOS);
					break;

				case 4:
					out(END_LEDS, END_5_LEDS_LIGADOS);
					break;

				case 5:
					out(END_LEDS, END_6_LEDS_LIGADOS);
					break;

				case 6:
					out(END_LEDS, END_7_LEDS_LIGADOS);
					break;

				case 7:		
					out(END_LEDS, END_8_LEDS_LIGADOS);
					break;
		}// Switch leds.
}



/*################################################################*/
/*
     Rotinas de Interrupção
 */
/*################################################################*/

/******************************************************************/
/* Função: IniciaSensorLed																				*/
/******************************************************************/
/* Função de interrupção que atualiza os sensores.*/
INTERRUPT (32820) IniciaSensorLed ()
{
/* Para quando o robo estiver funcionando.*/

		// Le e guarda o valor lido do sensor 1.
		if (MostraSensor1 == TRUE)
		{
			out(END_ROBOT, END_RD_GP2D_1);
			ValorSensor1 = in(END_ROBOT);
		}		

		// Le e guarda o valor lido do sensor 2.
		if (MostraSensor2 == TRUE)
		{		
			out(END_ROBOT, END_RD_GP2D_2);
			ValorSensor2 = in(END_ROBOT);
		}
		
		// Le e guarda o valor lido do sensor 3.
		if (MostraSensor3 == TRUE)
		{		
			out(END_ROBOT, END_RD_GP2D_3);
			ValorSensor3 = in(END_ROBOT);
		}
		
		// Le e guarda o valor lido do sensor 4.
		if (MostraSensor4 == TRUE)
		{		
			out(END_ROBOT, END_RD_GP2D_4);
			ValorSensor4 = in(END_ROBOT);
		}

}



/*################################################################*/
/*
     Main
 */
/*################################################################*/
main ()
{
	int tecla, i, ValorSensor;
	char Sensor;
	char s[TAM_MSG_TEXTO];

/******************************************************************
	Inicializa as Variaveis Globais.
******************************************************************/
	ValorSensor1 = 0;
	ValorSensor2 = 0;
	ValorSensor3 = 0;
	ValorSensor4 = 0;
	MostraSensor1 = FALSE;
	MostraSensor2 = FALSE;
	MostraSensor3 = FALSE;
	MostraSensor4 = FALSE;
	TeclaPressionada = 0;
	FinalizaRobo = FALSE;

/*****************************************************************/


/******************************************************************
	Inicializa as Variaveis Locais.
******************************************************************/
	ValorSensor = 0;
/*****************************************************************/


/******************************************************************
	Inicializa o visor LCD.
******************************************************************/
	// 8 bits, 2 linhas, matrix 5x10.
	out (END_DSPL, END_DSPL_CONFIG); 

	for (i=0 ; i<TEMPO_ESP_INIC; i++);
	// Desliga o display.
	out (END_DSPL, END_DSPL_DESLIGA); 

	for (i=0 ; i<TEMPO_ESP_INIC; i++);
	// Limpa memoria e contador.
	out (END_DSPL, END_DSPL_LIMPA); 

	for (i=0 ; i<TEMPO_ESP_INIC; i++);
	// Incrementa contador.
	out (END_DSPL, END_DSPL_INC); 

	for (i=0 ; i<TEMPO_ESP_INIC; i++);
	// Liga display com cursor piscando.
	out (END_DSPL, END_DSPL_LIGA); 

	for (i=0 ; i<TEMPO_ESP_INIC; i++);
  // Aponta para o endereco 80h - primeira posicao da linha 1.
	out (END_DSPL, END_DSPL_PRIM_POS_L_1); 
/*****************************************************************/
printf ("Inicializa Inter \n\n");

/******************************************************************
	Inicializa as interupções.
******************************************************************/

	// Marca o tempo das interrupções.
	out(END_ROBOT, END_TMR_MS);
	out(END_ROBOT, TEMPO_INTERRUPCAO);
	//Inicializa o timer.
	out (END_ROBOT, END_TMR_ON);
printf ("Inicializa Inter \n\n");	
	//desmascara
	  asm {
	    mvi A,$1D
	    sim
	  }
printf ("Inicializa Inter \n\n");	  
	//Habilita
    	enable();	
printf ("Inicializa Inter \n\n");    	
/*****************************************************************/

printf ("Inicia Loop\n\n");
/******************************************************************
		Loop Principal.
******************************************************************/
	while (TRUE)
	{
	/*****************************************************************
			Verifica se há alguma tecla pressionada.
	*****************************************************************/
		// Pega ultima tecla pressionada.
		tecla = in(END_TECLAS);
	/****************************************************************/

	/*******************************************************************
			Sinaliza os flags de teclas.
	*******************************************************************/
		switch (TeclaPressionada)
		{
			case VALOR_TECLA_1: // Tecla verifica o sensor 1
				MostraSensor1 = TRUE;
				break;

			case VALOR_TECLA_2:
				MostraSensor2 = TRUE;
				break;

			case VALOR_TECLA_3:
				MostraSensor3 = TRUE;
				break;

			case VALOR_TECLA_4:
				MostraSensor4 = TRUE;
				break;

			case VALOR_TECLA_5:
				break;

			case VALOR_TECLA_6:
				break;

			case VALOR_TECLA_7:
				break;

			case VALOR_TECLA_8:// Tecla de Saida.
				FinalizaRobo = TRUE;
				break;

		}// Switch.
	/******************************************************************/


	/*****************************************************************
			Posiciona e escreve o sensor no display e nos leds.
	*****************************************************************/
	if (MostraSensor1)
		{
			Sensor = '1';
			ValorSensor = ValorSensor1;
		}
	if (MostraSensor2)
		{
			Sensor = '2';
			ValorSensor = ValorSensor2;
		}
	if (MostraSensor3)
		{
			Sensor = '3';
			ValorSensor = ValorSensor3;
		}
	if (MostraSensor4)
		{
			Sensor = '4';
			ValorSensor = ValorSensor4;
		}

	//Verifico se devo escrever Sensor 1 no display
	if ((MostraSensor1 == TRUE) || (MostraSensor2 == TRUE) ||
	    (MostraSensor3 == TRUE) || (MostraSensor4 == TRUE)
	   )
	{
		setCursorLCD(1,8);
		if (in(END_DSPL_READ) != Sensor)
		{
			setCursorLCD(1,1);
			sprintf(s, "Sensor %c", Sensor);
			putStrLCD(s);
		}
	}
	if (ValorSensor != 0)
	{
		MostraSensorLed (ValorSensor);
	}
	printf("%d %d %d %d\n", ValorSensor1, ValorSensor2, ValorSensor3, ValorSensor4);
	/****************************************************************/


	/*****************************************************************
			Finalizo o robo, a interrupção, os leds e o display.
	*****************************************************************/
	if (FinalizaRobo == TRUE)
	{
		// Para o Robo.
		out(END_ROBOT, STOP);

		// Desabilita Interrupções.
		  disable();
		  out (0xf0, 0x51);
		
		// Apaga os Leds.
		out (END_LEDS, END_0_LEDS_LIGADOS);

		while (getStatusLCD());	// Aguarda o display ser liberado.
		// Desliga o display.
		out (END_DSPL, END_DSPL_DESLIGA); 

		break;
	}
	/****************************************************************/

	/*****************************************************************
			Confirma tecla pressionada e reinicia os flags.
	*****************************************************************/
	if(tecla) 
		{
			if( (tecla == in(END_TECLAS)) )
			{
					TeclaPressionada = tecla;

					// Reseta os flags de Controle de sensor
					MostraSensor1 = FALSE;
					MostraSensor2 = FALSE;
					MostraSensor3 = FALSE;
					MostraSensor4 = FALSE;
			}
		}
	/******************************************************************/

	}// While Principal.
/******************************************************************/
	
}

