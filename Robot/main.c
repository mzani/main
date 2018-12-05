/*****************************************************************/
/*      Programa........: Robot																		*/
/*----------------------------------------------------------------*/
/*      M�dulo..........: robot.c																	*/
/*----------------------------------------------------------------*/
/*      In�cio..........: 01/03/2007															*/
/*      Autor...........: Marcelo Tadeu Marchesoni Zani						*/
/*----------------------------------------------------------------*/
/*      �ltima altera��o: 05/12/2007															*/
/*      Respons�vel.....: Marcelo Tadeu Marchesoni Zani						*/
/******************************************************************/



/*################################################################*/
/*
		Informa��es Gerais.
*/
/*################################################################*/
/*
		-> Para melhor visualiza��o do c�digo utilize tabs com 
			 espa�amento de 2.
		-> A fun��o out escreve o segundo par�metro no endere�o de 
			 mem�ria do primeiro par�metro.
		-> A fun��o in l� o endere�o de mem�ria passado.
*/
/******************************************************************/



/*################################################################*/
/*
     Includes
 */
/*################################################################*/
#include "8085io.h"
#include "8085int.h"

#include "structs.h"

//#define _DEBUG
#define _DEBUG_2
/******************************************************************/


/*################################################################*/
/*
     Vari�veis

		-> H� um grande numero de vari�veis globais pois a estrutura 
		de um loop principal exige que muita delas sejam setadas e 
		verificadas durante sua execu��o.
*/
/*################################################################*/
/*################################################################*/
/*
     Variaveis Globais
 */
/*################################################################*/

// Guarda a tecla pressionada.
int TeclaPressionada;
// Flag - Bot�o Aumentar pressionado.
int Aumentar;
// Flag - Bot�o Diminuir pressionado.
int Diminuir;
// Flag - Bot�o Digito pressionado.
int Digito;
// Flag - Bot�o Finalizar pressionado.
int FinalizaRobo;
// Variavel respons�vel pelo Modo do Menu.
int MenuModo;
// Variavel respons�vel pelo N�vel do Menu.
int MenuNivel;
// Variavel respons�vel pelo Desvio Reto.
int DesvioReto;
// Variavel responsavel pelo Giro do Robo.
int GiraRobo;
// Verifica a exist�ncia de um obst�culo.
int VerificaObstaculo;
// Atualiza o tempo para confinar o rob� numa �rea.
int AtualizaTempo;
// Conta o n�mero de interrup��es.
int ContInter;

// Conjunto de Vari�veis responsaveis pelos valores dos par�metros.
int Velocidade;
int Distancia;
int Graus;
int Encoder;
int VerificaSensor;

// Struct para os sensores.
struct SENSOR Sensor [NUM_SENSORES];
/******************************************************************/


/*################################################################*/
/*
     Rotinas

		-> Poucas fun��es foram criadas visando agilizar o loop 
		principal, j� que chamar uma fun��o causa	uma maior lentid�o. 
		Isso foi priorizado em detrimento da melhor leitura do 
		c�digo.
*/
/*################################################################*/
/*################################################################*/
/*
     Rotinas para Debug
 */
/*################################################################*/
/******************************************************************/
/* Fun��o: Debug																									*/
/******************************************************************/
int Debug (char *s)
{
#ifdef _DEBUG
int digitado;

digitado = 'a';

printf ("%s\n\n", s);

while (digitado != 'c')
	digitado = chkchr();

printf ("Continua \n\n");

return 0;
#endif
}
/******************************************************************/



/******************************************************************/
/* Fun��o: Debug_int																							*/
/******************************************************************/
Debug_int (char *s, int n)
{
#ifdef _DEBUG_2
printf ("%s: %d\n\n", s, n);
#endif
}
/******************************************************************/



/******************************************************************/
/* Fun��o: Debug_char																							*/
/******************************************************************/
Debug_char (char *s, char c)
{
#ifdef _DEBUG_2
printf ("%s: %c\n\n", s, c);
#endif
}
/******************************************************************/



/*################################################################*/
/*
     Rotinas Gerais
 */
/*################################################################*/
/******************************************************************/
/* Fun��o: pow																										*/
/******************************************************************/
/* Calcula um n�mero elevado a uma pot�ncia */
int pow(int numero, int exp)
{
	int i;
	int resultado;
	
	for(i=0 ; i<exp; i++)
		resultado*=numero;
		
	return resultado;
}
/******************************************************************/



/******************************************************************/
/* Fun��o: MostraSensorLed																				*/
/******************************************************************/
/* Escreve os leds no display */
MostraSensorLed(int ValorMedido)
{
	int ValorLed, i;

	// Divide para poder mostrar nos 8 leds
	ValorLed = ValorMedido/VALOR_ESCALA;

	// Espera um tempo entre um comando e outro.
	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);

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
/******************************************************************/



/*################################################################*/
/*
     Rotinas de display
 */
/*################################################################*/
/******************************************************************/
/* Fun��o: getStatusLCD																						*/
/******************************************************************/
/* Verifica se o visor est� livre*/
int getStatusLCD () 
{
	// Retorna o flag de busy.
	return (in(END_DSPL_BUSY) & END_FLG_BUSY) != 0; 
}
/******************************************************************/



/******************************************************************/
/* Fun��o: setCursorLCD																						*/
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



/******************************************************************/
/* Fun��o: putChrLCD																							*/
/******************************************************************/
/* Escreve um char no display na posicao atual do cursor */
putChrLCD (char c) 
{
	while(getStatusLCD()); // Aguarda o display desocupar.
	out(END_DSPL_WRITE, c);
}
/******************************************************************/



/******************************************************************/
/* Fun��o: putStrLCD																							*/
/******************************************************************/
/* Escreve uma string no display a partir da posicao atual */
putStrLCD (char *s)
{
	int i;
	for(i=0; s[i] != '\0'; i++)
		putChrLCD (s[i]);
}
/******************************************************************/



/******************************************************************/
/* Fun��o: putCleanStrLCD																					*/
/******************************************************************/
/* Limpa e escreve na linha a string passada */
putCleanStrLCD (char *s, int L)
{
	// Limpa o visor.
	setCursorLCD(L, 1);
	putStrLCD("                ");
	// Escreve no visor.
	setCursorLCD(L, 1);
	putStrLCD(s);
}
/******************************************************************/


/*################################################################*/
/*
     Rotina de Interrup��o

		-> Essa rotina � executada a cada pulso configurado na 
		inicializa��o. Ela � independente da execu��o do main.
*/
/*################################################################*/
/*################################################################*/
/*
     Rotinas de Interrup��o
 */
/*################################################################*/
/******************************************************************/
/* Fun��o: IniciaSensorLed																				*/
/******************************************************************/
/* Fun��o de interrup��o para a atualiza��o dos sensores.*/
INTERRUPT (32820) IniciaSensorLed ()
{
	int i;

    // Espera um tempo entre um comando e outro.
 		for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
  	
		// Verifica qual sensor deve atualizar.
		if(VerificaSensor == 1)
		{
  		// Le e guarda o valor lido do sensor 1.                      
  		out(END_ROBOT, END_RD_GP2D_1);
    }
		if(VerificaSensor == 2)
		{		
  		// Le e guarda o valor lido do sensor 2.
  		out(END_ROBOT, END_RD_GP2D_2);
    }
		if(VerificaSensor == 3)
		{    
  		// Le e guarda o valor lido do sensor 3.
  		out(END_ROBOT, END_RD_GP2D_3);
    }
		if(VerificaSensor == 4)
		{    
  		// Le e guarda o valor lido do sensor 4.
  		out(END_ROBOT, END_RD_GP2D_4);
    }

		// L� o valor do sensor escolhido.
    if (VerificaSensor > 0)
       Sensor[VerificaSensor-1].ValorLido = in(END_ROBOT);
		
		if (AtualizaTempo == TRUE)
			ContInter +=1;
			
			printf("\nContInter: %d\n", ContInter);

}
/******************************************************************/



/*################################################################*/
/*
     Main
 */
/*################################################################*/
main ()
{
	// Mensagem auxiliar.
	char s[TAM_MSG_TEXTO];
	int tecla, i, NumDigitos, tempoTecla, d;
	int num_h, num_l;
	int ang_h, ang_l;
	int enc_h, enc_l;
	int VerificaTecla;
	int QteMaxDigitos, *Parametro, ValorMaxPar, ValorMinPar;
	int ValorEncR;
	int ValorEncL;
	int ValorMediaEnc;
	int DistanciaTotal;
	int NumSensor;
	int ContadorEncoder;
	int TempoGiro;
	int VelocidadeGiro;
	int DistAnt, DistAtual;
	int ContLoop;
	int TesteSensor;

/******************************************************************
	Inicializa as Variaveis Globais.
******************************************************************/

	MenuModo = MOVE_MM;
	Aumentar = FALSE;
	Diminuir = FALSE;
	Digito = 0;
	FinalizaRobo = FALSE;
	MenuNivel = 1;
	Velocidade = 0;
	Distancia = 0;
	Graus = 0;
	Encoder = 0;
	VerificaSensor = 0;
	TesteSensor = 0;
	TeclaPressionada = 0;
	DesvioReto = FALSE;
	GiraRobo = FALSE;
	VerificaObstaculo = FALSE;
	AtualizaTempo = FALSE;
	ContInter = 0;
	ContLoop = 0;
	TempoGiro = 0;
	VelocidadeGiro = 0;
/*****************************************************************/


/******************************************************************
	Inicializa as Variaveis Locais.
******************************************************************/
	NumDigitos=0;
	tempoTecla = 0;
	VerificaTecla = TRUE;
	DistanciaTotal = 0;
	ContadorEncoder = ESPERA_ENCODER;
	DistAnt= 0;
  DistAtual=0;
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

	// Mostra o Menu Inicial.
	putCleanStrLCD("MOVE_MM", 1);
	putCleanStrLCD("(Vel e Dist)", 2);
	
/*****************************************************************/

/******************************************************************
	Inicializa as interup��es.
******************************************************************/
/* 
// No caso de utiliza��o das interrup��es para atualiza��o dos
// sensores deve-se habilitar essa parte do c�digo.

	// Marca o tempo das interrup��es.

	// Espera um tempo entre um comando e outro.
	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
	out(END_ROBOT, END_TMR_MS);
	
	// Espera um tempo entre um comando e outro.
	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
	// 50 ms
	out(END_ROBOT, TEMPO_INTERRUPCAO); 
	
	// Espera um tempo entre um comando e outro.
	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
	// Inicializa o timer.
	out(END_ROBOT, END_TMR_ON);
	
	// Desmascara
	  asm {
	    mvi A,$1D
	    sim
	  }

	// Habilita
    	enable();	

Debug("Desmacaro");

*/
/*****************************************************************/

/******************************************************************
	Inicializa o Enconder.
******************************************************************/
	// Espera um tempo entre um comando e outro.
	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
	// Inicializa os Encoders.
	out(END_ROBOT, END_RESET_ENC_ALL);
/*****************************************************************/

// _DEBUG
Debug("Inicio Encoder");

/******************************************************************
		Loop Principal.
******************************************************************/
/*
		Algoritmos de Rob�s costumam ficar em um loop principal, pois
		a todo momento, eles precisam atualizar e verificar flags
		para a rea��o em tempo real.

		Exemplo: Enquanto o rob� desvia � desejavel que os bot�es sejam
		verificados.

*/
	while (TRUE)
	{
	/*****************************************************************
			Verifica se h� alguma tecla pressionada.
	*****************************************************************/
	// Espera um tempo entre um comando e outro.
	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);				
	// Pega ultima tecla pressionada.
		tecla = in(END_TECLAS);
	/****************************************************************/


	/*******************************************************************
			Sinaliza os flags de teclas.
	*******************************************************************/
		switch (TeclaPressionada)
		{
			case VALOR_TECLA_1: // Tecla para finalizar o Robo.
				FinalizaRobo = TRUE;
				TeclaPressionada = 0;
				break;
			case VALOR_TECLA_2:// Tecla para Diminuir.
				Diminuir = TRUE;
				TeclaPressionada = 0;
				break;
			case VALOR_TECLA_3:// Tecla voltar um nivel.
				MenuNivel -=1;
				TeclaPressionada = 0;				
				break;
			case VALOR_TECLA_4:// Tecla retornar ao menu inicial.
				MenuNivel=1;
				TeclaPressionada = 0;				
				break;
			case VALOR_TECLA_5:// Tecla para mudar o digito selecionado.
				Digito += 1;
				TeclaPressionada = 0;				
				break;
			case VALOR_TECLA_6:// Tecla para Aumentar.
				Aumentar = TRUE;
				TeclaPressionada = 0;				
				break;
			case VALOR_TECLA_7:// Tecla para avan�ar um nivel.
				MenuNivel +=1;
				TeclaPressionada = 0;				
				break;
			case VALOR_TECLA_8:// Tecla para mudar de modo.
				if (MenuNivel == 1)
					MenuModo +=1;
				TeclaPressionada = 0;				
				break;
		}// Switch.
	/*****************************************************************/

	// _DEBUG

	Debug("Verifico Tecla");

	/*****************************************************************
			Menu do programa.
	*****************************************************************/

	/* Verifica��o Modo m�nimo */
	if (MenuModo <= START_NIVEL)
		{
			MenuModo = STOP;
		}

	/* Verifica��o Modo m�ximo */
	if (MenuModo > RESET_MODO)
		{
			MenuModo = MOVE_MM;
		}

	/* Verifica��o Nivel m�nimo */
	if (MenuNivel <= START_NIVEL)
		{
			MenuNivel = NIVEL_INICIAL;
		}

	/* Voltou ao Menu inicializa as v�riaveis dos par�metros*/
	if (MenuNivel == NIVEL_INICIAL)
		{
			Velocidade = 0;
			Distancia = 0;
			Graus = 0;
			Encoder = 0;			
			VerificaSensor = 0;
			DesvioReto = FALSE;
			VerificaObstaculo = FALSE;
			GiraRobo = FALSE;
			AtualizaTempo = FALSE;
			Digito = 0;
		}


	/*****************************************************************
			N�vel 1.
	*****************************************************************/

	/* Modo MOVE_MM */
	if ((MenuModo == MOVE_MM) && (MenuNivel == 1) && (tecla != 0)) 
		{
			putCleanStrLCD("MOVE_MM", 1);
			putCleanStrLCD("(Vel e Dist)", 2);
		}

	/* Modo MOVE_SPD */
	if ((MenuModo == MOVE_SPD) && (MenuNivel == 1) && (tecla != 0))
		{
			putCleanStrLCD("MOVE_SPD", 1);
			putCleanStrLCD("(Vel)", 2);
		}
	
	/* Modo TURN_DEG */
	if ( (MenuModo == TURN_DEG) && (MenuNivel == 1) && (tecla != 0))
		{
			putCleanStrLCD("TURN_DEG", 1);
			putCleanStrLCD("(Vel e Graus)", 2);
		}

	/* Modo TURN_SPD */
	if ( (MenuModo == TURN_SPD) && (MenuNivel == 1) && (tecla != 0))
		{
			putCleanStrLCD("TURN_SPD", 1);
			putCleanStrLCD("(Vel)", 2);
		}

	/* Modo R_WHEEL_SPD */
	if ( (MenuModo == R_WHEEL_SPD) && (MenuNivel == 1) && (tecla != 0))
		{
			putCleanStrLCD("R_WHEEL_SPD", 1);
			putCleanStrLCD("(Vel)", 2);
		}

	/* Modo L_WHEEL_SPD */
	if ( (MenuModo == L_WHEEL_SPD) && (MenuNivel == 1) && (tecla != 0))
		{
			putCleanStrLCD("L_WHEEL_SPD", 1);
			putCleanStrLCD("(Vel)", 2);
		}

	/* Modo TESTE_SENSORES */
	if ((MenuModo == TESTE_SENSORES) && (MenuNivel == 1) && (tecla != 0)) 
		{
			putCleanStrLCD("TESTE SENSORES", 1);
			putCleanStrLCD("(Sensor)", 2);
		}

	/* Modo MAIN_AUTO */
	if ((MenuModo == MAIN_AUTO) && (MenuNivel == 1) && (tecla != 0)) 
		{
			putCleanStrLCD("MODO AUTOMATICO", 1);
			putCleanStrLCD("(-)", 2);
		}

	/* Modo STOP */
	if ( (MenuModo == STOP) && (MenuNivel == 1) && (tecla != 0))
		{
			putCleanStrLCD("STOP", 1);
			putCleanStrLCD("(-)", 2);
		}


	/*****************************************************************
			N�vel 2.
	*****************************************************************/

	/******************************************************************/
	/* Atualiza Sensores																							*/
	/******************************************************************/	
		if ( ((MenuModo == TESTE_SENSORES) || (MenuModo == MAIN_AUTO)) && 
        (MenuNivel >= 2) )
     {

       for(VerificaSensor = 0; VerificaSensor < NUM_SENSORES; VerificaSensor ++)
       {
        // Espera um tempo entre um comando e outro.
     		for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
      	
    		// Verifica qual sensor deve atualizar.
    		if(VerificaSensor == 0)
    		{
      		// Le e guarda o valor lido do sensor 1.                      
      		out(END_ROBOT, END_RD_GP2D_1);
        }
    		if(VerificaSensor == 1)
    		{		
      		// Le e guarda o valor lido do sensor 2.
      		out(END_ROBOT, END_RD_GP2D_2);
        }
    		if(VerificaSensor == 2)
    		{    
      		// Le e guarda o valor lido do sensor 3.
      		out(END_ROBOT, END_RD_GP2D_3);
        }
    		if(VerificaSensor == 3)
    		{    
      		// Le e guarda o valor lido do sensor 4.
      		out(END_ROBOT, END_RD_GP2D_4);
        }
    
        // Espera um tempo entre um comando e outro.
     		for (i=0 ; i<TEMPO_ESP_COMANDO; i++);	
     		
    		// L� o valor do sensor escolhido.
        if (VerificaSensor >= 0)
           Sensor[VerificaSensor].ValorLido = in(END_ROBOT);
    		
				// Conta o n�mero de loops dados.
      	if (AtualizaTempo == TRUE)
    			ContLoop +=1;
        }
     }

	/******************************************************************/
	/* Modo STOP																											*/
	/******************************************************************/	
	if ( (MenuModo == STOP) && (MenuNivel == 2) && (tecla != 0))
		{
		/* Verifica��o Nivel m�ximo */
		if (MenuNivel >= 2)
			{
				MenuNivel = 2;
			} 
             
			/*Comandos Parar*/
			putCleanStrLCD("STOP Confirmado", 1);
			putCleanStrLCD("<- Retorne ", 2);

			// Espera um tempo entre um comando e outro.
			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);	
			out(END_ROBOT, STOP);
		}
	/*****************************************************************/

	// _DEBUG
	Debug("Nivel 2: Passou do Stop");

	/******************************************************************/
	/* Modo TESTE_SENSORES																						*/
	/******************************************************************/	
	if ( (MenuModo == TESTE_SENSORES) && (MenuNivel == 2) )
		{
		/* Verifica��o Nivel m�ximo */
		if (MenuNivel >= 2)
			{
				MenuNivel = 2;
			}        

			/* Variaveis para verifica��es e atualiza��es */
			QteMaxDigitos = 1;
			Parametro = &TesteSensor;
			ValorMaxPar = VALOR_MAX_SENSOR;
			ValorMinPar = VALOR_MIN_SENSOR;

     if (tecla !=0)
     {
    		sprintf(s, "Teste Sensor %d", TesteSensor);
    		putCleanStrLCD(s, 1);
    		putCleanStrLCD("<- Retorne ", 2);
     }
     
		 // _DEBUG_2
		 /* 
		 // DUMP
				sprintf(s, "Valor Sensor %d: ", VerificaSensor);
				Debug_int(s, Sensor[(VerificaSensor-1)].ValorLido);
			*/

			// Mostra o valor lido no sensor nos leds.
			MostraSensorLed (Sensor[(TesteSensor-1)].ValorLido);
		}
	/*****************************************************************/

	// _DEBUG
	Debug("Nivel 2: Teste dos Sensores");

	/******************************************************************/
	/* Velocidade																											*/
	/******************************************************************/	
	if ( ( (MenuModo == MOVE_MM)     || (MenuModo == MOVE_SPD) || 
			   (MenuModo == TURN_SPD)    || (MenuModo == TURN_DEG) || 
			   (MenuModo == L_WHEEL_SPD) || (MenuModo == R_WHEEL_SPD) ) &&
		     (MenuNivel == 2)
		 ) 
		{
			/* Variaveis para verifica��es e atualiza��es */
			QteMaxDigitos = 1;
			Parametro = &Velocidade;
			ValorMaxPar = VEL_MAX;
			ValorMinPar = VEL_MIN;

			/* Executa o comando selecionado */
			if ((MenuModo != MOVE_MM) && (MenuModo != TURN_DEG))
			{
				// Espera um tempo entre um comando e outro.
				for (i=0 ; i<TEMPO_ESP_COMANDO; i++);					
				out(END_ROBOT, MenuModo);

				// Espera um tempo entre um comando e outro.
				for (i=0 ; i<TEMPO_ESP_COMANDO; i++);	
				out(END_ROBOT, Velocidade);
			}

			/* Escreve no visor */
			if ((tecla != 0))
			{
				setCursorLCD(1,1);
				putCleanStrLCD("Velocidade", 1);

				if (Velocidade >= 0)
					sprintf(s, "+%02d nivel", Velocidade);
				else
					sprintf(s, "%03d nivel", Velocidade);

				putCleanStrLCD(s, 2);	
			}
		}
	/*****************************************************************/

	// _DEBUG
	Debug("Nivel 2: Pega Velocidade");


	/******************************************************************/
	/* Modo MAIN_AUTO																									*/
	/******************************************************************/	
	if ((MenuModo == MAIN_AUTO) && (MenuNivel == 2) && (tecla != 0))
		{

			setCursorLCD(1,1);
			putCleanStrLCD("Moto Automatico", 1);
			putCleanStrLCD("", 2);

			//  Inicializa os Encoders.
			//  Espera um tempo entre um comando e outro.
			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
		  out(END_ROBOT, END_RESET_ENC_ALL);
			
			// Inicia o movimento do rob�.
			DesvioReto = TRUE;

			//  Habilita a atualiza��o do contador.
			AtualizaTempo = TRUE;
		}
	/*****************************************************************/

	// _DEBUG
	Debug("Nivel 2: Auto");		

	/*****************************************************************
			Tempo Limite
	*****************************************************************/
	if ((MenuModo == MAIN_AUTO) && (MenuNivel >= 2) 
		 )
	{
		//  Tempo limite est� em segundos.
		if ((ContLoop >= TEMPO_LIMITE) )
		{
			// Espera um tempo entre um comando e outro.
			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
			out(END_ROBOT, TURN_SPD);

			// Espera um tempo entre um comando e outro.
			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);
			out(END_ROBOT, VEL_GIRO_ANT_HOR);
      
			// Gira o tempo necess�rio.
   		for (i=0 ; i<TEMPO_GIRO_LIMITE; i++);		

  		// Espera um tempo entre um comando e outro.
  		for (i=0 ; i<TEMPO_ESP_COMANDO; i++);				
  		out(END_ROBOT, MOVE_SPD);
  
  		// Espera um tempo entre um comando e outro.
  		for (i=0 ; i<TEMPO_ESP_COMANDO; i++);
  		out(END_ROBOT, VEL_DESVIO);
  		
  		ContLoop = 0;
  		
		}
	}
	/****************************************************************/

	// _DEBUG
	Debug("Nivel 2: Atualiza Tempo");

	/*****************************************************************
			Desvio Reto.
	*****************************************************************/
	if ((MenuModo == MAIN_AUTO) && (MenuNivel >= 2) && (DesvioReto == TRUE))
	{
		// Intera��o com o usu�rio
		if (MenuNivel == 2)
		{
			putCleanStrLCD("Rob�",1);
			putCleanStrLCD("Livre",2);

			MostraSensorLed (Sensor[(NumSensor-1)].ValorLido);
		}

		// Espera um tempo entre um comando e outro.
		for (i=0 ; i<TEMPO_ESP_COMANDO; i++);				
		out(END_ROBOT, MOVE_SPD);

		// Espera um tempo entre um comando e outro.
		for (i=0 ; i<TEMPO_ESP_COMANDO; i++);
		out(END_ROBOT, VEL_DESVIO);

		 if ((Sensor[0].ValorLido <= VALOR_SENSOR_MINIMO) &&
				 (Sensor[1].ValorLido <= VALOR_SENSOR_MINIMO) &&
				 (Sensor[2].ValorLido <= VALOR_SENSOR_MINIMO) &&
				 (Sensor[3].ValorLido <= VALOR_SENSOR_MINIMO)
				)
		{
			DesvioReto = FALSE;

			for (i=0 ; i<5000; i++);

			VerificaObstaculo = TRUE;
		}

		/* 
		// Dump
		sprintf(s, "DesvioReto: Sensores %d %d %d %d", Sensor[0].ValorLido, 
																			 Sensor[1].ValorLido, 
																			 Sensor[2].ValorLido, 
																			 Sensor[3].ValorLido);
    Debug_int(s, 0);
		*/
	}
	
	/****************************************************************/
	
	// _DEBUG
  Debug("Nivel 2: Desvio Reto");		     
	  
	/*****************************************************************
			Verifica obstaculos
	*****************************************************************/
		 if ( (MenuModo == MAIN_AUTO) && (MenuNivel >= 2) && (VerificaObstaculo == TRUE) )
		 {
      for(NumSensor=0; NumSensor<NUM_SENSORES ; NumSensor++)
			{
					// Verifica qual � o sensor que tem obst�culo.
					if ((Sensor[NumSensor].ValorLido > VALOR_SENSOR_MINIMO))
					{
						// Verifica qual � o obstaculo.
            if (NumSensor == 0)
						{
							 VelocidadeGiro = VEL_GIRO_ANT_HOR;
							 TempoGiro = 0;
						}

            if (NumSensor == 1)
						{
    				  
							VelocidadeGiro = VEL_GIRO_HOR;

							// Espera um tempo entre um comando e outro.
							for (i=0 ; i<TEMPO_ESP_COMANDO; i++);
							// Robo para onde estiver.
							out(END_ROBOT, STOP);
							
              TempoGiro = TEMPO_GIRO_OBST_ESQ + 1200;
						}

            if (NumSensor == 2)
						{
							VelocidadeGiro = VEL_GIRO_ANT_HOR;

							// Espera um tempo entre um comando e outro.
							for (i=0 ; i<TEMPO_ESP_COMANDO; i++);
							// Robo para onde estiver.
							out(END_ROBOT, STOP);
							
              TempoGiro = TEMPO_GIRO_OBST_DIR + 1200;
						}

            if (NumSensor == 3)
						{
							VelocidadeGiro = VEL_GIRO_ANT_HOR;

							// Espera um tempo entre um comando e outro.
							for (i=0 ; i<TEMPO_ESP_COMANDO; i++);
							// Robo para onde estiver.
							out(END_ROBOT, STOP);     
                       
							TempoGiro = TEMPO_GIRO_OBST_FRENTE;
						}
                                                            
      			GiraRobo = TRUE;
            VerificaObstaculo = FALSE;
      			
      		/* 
      		// Dump
       			sprintf(s, "VerificaObstaculo: Sensor %d ", NumSensor);
      	    Debug_int(s, Sensor[NumSensor].ValorLido);    
          */
      			
						break;
					}
			}


		 }
		 
	/****************************************************************/

	// _DEBUG
	Debug("Nivel 2: Verifica Obstaculo");		

	/*****************************************************************
			GiraRobo.
	*****************************************************************/
	if ((MenuModo == MAIN_AUTO) && (MenuNivel >= 2) && (GiraRobo == TRUE))
	{

		if (MenuNivel == 2)
		{
			putCleanStrLCD("Rob�", 1);
			putCleanStrLCD("Desviando", 2);			
		}
		
		if (TempoGiro >0)
		{
// Girar Inteligente.
///*
		
		// Escolhe o sensor a ser verificado.
    if (NumSensor == 3)
       NumSensor =2;
              
//		if (DistAnt <= (DistAtual))
//			{
//				DistAnt = DistAtual;
				// Espera um tempo entre um comando e outro.
				for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
				out(END_ROBOT, TURN_SPD);
			
				// Espera um tempo entre um comando e outro.
				for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
				out(END_ROBOT, VelocidadeGiro);
				
//				for (i=0 ; i<TEMPO_GIRO_INTELIGENTE; i++);
				for (i=0 ; i<TempoGiro; i++);

  			// Espera um tempo entre um comando e outro.
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);	
  			out(END_ROBOT, STOP);       
  			
				DistAtual = Sensor[(NumSensor)].ValorLido;

/*			}
			else
			{
*/      	
        	// Voltar para a posi��o anterior.
      		// Espera um tempo entre um comando e outro.
      		for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
      		out(END_ROBOT, TURN_SPD);
      	
      		// Espera um tempo entre um comando e outro.
      		for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
      		// Inverte o sentido de Giro
      		out(END_ROBOT, ((-1)*VelocidadeGiro));
      		
      		for (i=0 ; i<TEMPO_GIRO_INTELIGENTE; i++);
/*      		
      */
      
      	GiraRobo = FALSE;
      	DesvioReto = TRUE;
      	ContLoop = 0;
      	
      	DistAnt = 0;
        DistAtual = 0;
        
        MostraSensorLed (Sensor[(NumSensor-1)].ValorLido);
//      }
   }
  }
	
	/****************************************************************/

	// _DEBUG
	Debug("Nivel 2: Gira Robo");		
	
	/*****************************************************************
			N�vel 3.
	*****************************************************************/

	/* Dist�ncia */
	if ( (MenuModo == MOVE_MM) && 
	     (MenuNivel == 3)
	   ) 
		{
  
			/* Variaveis para verifica��es e atualiza��es */
			QteMaxDigitos = 4;
			Parametro = &Distancia;
			ValorMaxPar = DIST_MAX;
			ValorMinPar = DIST_MIN;

			/* Escreve no visor */
			if ((tecla != 0))
			{
				setCursorLCD(1,1);
				putCleanStrLCD("Distancia", 1);

				if (Distancia >= 0)
					sprintf(s, "+%05d mm", Distancia);
				else
					sprintf(s, "%06d mm", Distancia);

				putCleanStrLCD(s, 2);
			}
			
		}

	// _DEBUG
	Debug("Nivel 3: Pega Distancia");		

	/* Graus */
	if ( (MenuModo == TURN_DEG) && 
		   (MenuNivel == 3)       && 
		   (tecla != 0)
	   ) 
		{
		/* Verifica��o Nivel m�ximo */
		if (MenuNivel >= 3)
			{
				MenuNivel = 3;
			}                       
			/* Variaveis para verifica��es e atualiza��es */
			QteMaxDigitos = 4;
			Parametro = &Graus;
			ValorMaxPar = GRAU_MAX;
			ValorMinPar = GRAU_MIN;

			/* Escreve no visor */
			if ((tecla != 0))
			{
				setCursorLCD(1,1);
				putCleanStrLCD("Graus", 1);

				if (Graus >=0)
					sprintf(s, "+%05d", Graus);
				else
					sprintf(s, "%06d", Graus);

				putCleanStrLCD(s, 2);
			}
			
		}

	// _DEBUG
	Debug("Nivel 3: Pega Graus");		

	/* Encoder */
	if ( ( (MenuModo == MOVE_SPD)      ||
		     (MenuModo == TURN_SPD)      ||
		     (MenuModo == L_WHEEL_SPD)   ||
		     (MenuModo == R_WHEEL_SPD) 
		 )                               && 
		     (MenuNivel >= 3)
	   ) 
		{
      ContadorEncoder +=1;
             
			/* Verifica��o Nivel m�ximo */
			if (MenuNivel >= 3)
				{
					MenuNivel = 3;
				}

			setCursorLCD(1,1);

      if (ContadorEncoder >= ESPERA_ENCODER)
      {
  			// Espera um tempo entre um comando e outro.
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);	
  			// Leia encoder da roda esquerda.
  			out(END_ROBOT, END_RD_ENC_L_2B);
   			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);	
  			enc_h = in(END_ROBOT);
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);	  			
  			enc_l = in(END_ROBOT);
  			
  			sprintf(s, "Enc L: %d", (enc_h*256) + enc_l);
  			putCleanStrLCD(s,1);
  
  			// Espera um tempo entre um comando e outro.
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
  			// Leia encoder da roda direita.
  			out(END_ROBOT, END_RD_ENC_R_2B);
   			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);	
  			enc_h = in(END_ROBOT);
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);	  			
  			enc_l = in(END_ROBOT);
  			
  			sprintf(s, "Enc R: %d", (enc_h*256) + enc_l);
  			putCleanStrLCD(s,2);
     }
		}
	/*****************************************************************/

	// _DEBUG
	Debug("Nivel 3: Encoder");		

	/*****************************************************************
			N�vel 4.
	*****************************************************************/
  /* Encoder */
	if ( ( (MenuModo == MOVE_MM) || (MenuModo == TURN_DEG) || (MenuModo == MAIN_AUTO)) &&
		     (MenuNivel >= 4) && (tecla != 0)
		 )
		{
      ContadorEncoder +=1;
      
			/* Verifica��o Nivel m�ximo */
			if (MenuNivel >= 4)
				{
					MenuNivel = 4;
				}

			if ((MenuModo == MOVE_MM))
			{
				num_h = (Distancia>>8);
				num_l = Distancia - (num_h<<8);
			}

			if ((MenuModo == TURN_DEG))
			{
				num_h = (Graus>>8);
				num_l = Graus - (num_h<<8);
			}

			/* Executa o comando selecionado */
			// Espera um tempo entre um comando e outro.
			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
			out(END_ROBOT, MenuModo);

			// Espera um tempo entre um comando e outro.
			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
			out(END_ROBOT, num_h);
			
			// Espera um tempo entre um comando e outro.
			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
			out(END_ROBOT, num_l);

			// Espera um tempo entre um comando e outro.
			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);					
			out(END_ROBOT, Velocidade);

			setCursorLCD(1,1);

      if (ContadorEncoder >= ESPERA_ENCODER)
      {
  			// Espera um tempo entre um comando e outro.
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
  			out(END_ROBOT, END_RD_ENC_L_2B);
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);				
  			enc_h = in(END_ROBOT);
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);				
  			enc_l = in(END_ROBOT);
  			sprintf(s, "Enc L: %d", (enc_h*256) + enc_l);
  			putCleanStrLCD(s,1);
  
  			// Espera um tempo entre um comando e outro.
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
  			out(END_ROBOT, END_RD_ENC_R_2B);
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);							
  			enc_h = in(END_ROBOT);
  			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);							
  			enc_l = in(END_ROBOT);
  			sprintf(s, "Enc R: %d", (enc_h*256) + enc_l);
  			putCleanStrLCD(s,2);
     }
		}

	/****************************************************************/

	// _DEBUG
	Debug("Nivel 4: Encoder");		

	/*****************************************************************
			Atualiza��o de par�metros.
	*****************************************************************/
	
		if ((MenuNivel >= 2))
		{
			/* Se o Digito passar do mais significativo, ele volta ao 
				 primeiro */
			if (Digito > QteMaxDigitos)
				Digito = 0;
			
			/* Aumenta o Digito selecionado*/
			if (Aumentar == TRUE)
			{
				// Soma o devido digito selecinado.
				(*Parametro) +=(1 * pow(10, Digito) );

				// Verifica se n�o passou do m�ximo.
				if ((*Parametro) >= ValorMaxPar)
					(*Parametro) = ValorMaxPar;

				// Desmarca o Flag.
				Aumentar = FALSE;
			}

			/* Diminui o Digito selecionado*/
			if (Diminuir == TRUE)
			{
				// Subtrai o devido digito selecinado.
				(*Parametro) -=(1 * pow(10, Digito) );

				// Verifica se n�o passou do m�nimo.
				if ((*Parametro) <= ValorMinPar)
					(*Parametro) = ValorMinPar;

				// Desmarca o Flag.
				Diminuir = FALSE;
			}

			/* Especifico para o modo de TESTE_SENSORES*/
			// Marca o Digito 
			if ((tecla != 0) && (MenuModo != TESTE_SENSORES))
				setCursorLCD(2, ((QteMaxDigitos +2) - Digito ));
			
			// Posiciona o cursor
			if ((MenuModo == TESTE_SENSORES))
				setCursorLCD(1, 14);
				
			/* Especifico para o modo STOP*/
			// Posiciona o cursor
			if ((MenuModo == STOP))
				setCursorLCD(2, 1);				
				
				
		}
	/****************************************************************/

	// _DEBUG
	Debug("Atualiza Parametro");		

	/*****************************************************************
			Finalizo o robo, a interrup��o, os leds e o display.
	*****************************************************************/
	if (FinalizaRobo == TRUE)
	{
		// Para o Robo.
		out(END_ROBOT, STOP);

		// Desabilita Interrup��es.
		disable();
		
		// Apaga os Leds.
		out (END_LEDS, END_0_LEDS_LIGADOS);
		
		while (getStatusLCD());	// Aguarda o display ser liberado.
		// Desliga o display.
		out (END_DSPL, END_DSPL_DESLIGA); 

		break;
	}
	/****************************************************************/

	// _DEBUG
	Debug("Finaliza Robo");		

	/*****************************************************************
			Confirma tecla pressionada e verifica Bounce.
	*****************************************************************/
	if(tecla) 
		{
      tempoTecla +=1;
      if( (tecla == in(END_TECLAS)) && (tempoTecla >= TEMPO_BOUNCE) && (VerificaTecla == TRUE))
				{
					TeclaPressionada = tecla;
					tempoTecla = 0;
					VerificaTecla = FALSE;
				}
		}
		else
		{
			VerificaTecla = TRUE;
		}
	/******************************************************************/

	// _DEBUG
	Debug("Bounce");		

	}// While Principal.
/******************************************************************/
	
}
/******************************************************************/
