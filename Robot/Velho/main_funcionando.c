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

#include "structs.h"

//#define _DEBUG
#define _DEBUG_2

/*################################################################*/
/*
     Variaveis Globais
 */
/*################################################################*/

// Guarda a tecla pressionada.
int TeclaPressionada;
// Flag - Botão Aumentar pressionado.
int Aumentar;
// Flag - Botão Diminuir pressionado.
int Diminuir;
// Flag - Botão Digito pressionado.
int Digito;
// Flag - Botão Finalizar pressionado.
int FinalizaRobo;
// Variavel responsável pelo Modo do Menu.
int MenuModo;
// Variavel responsável pelo Nível do Menu.
int MenuNivel;
// Variavel responsável pelo Desvio Reto.
int DesvioReto;
// Variavel responsavel pelo Giro do Robo.
int GiraRobo;
// Verifica a existência de um obstáculo.
int VerificaObstaculo;
// Atualiza o tempo para confinar o robô numa área.
int AtualizaTempo;
// Conta o número de interrupções.
int ContInter;
// Variavel que avisa que o robo está em desvio.
int DesviaRobo;

// Conjunto de Variáveis responsaveis pelos valores dos parâmetros.
int Velocidade;
int Distancia;
int Graus;
int Encoder;
int VerificaSensor;

// Struct para os sensores.
struct SENSOR Sensor [NUM_SENSORES];


/*################################################################*/
/*
     Rotinas Gerais
 */
/*################################################################*/
/******************************************************************/
/* Função: pow																										*/
/******************************************************************/
/* Calcula um número elevado a uma potência */
int pow(int numero, int exp)
{
	int i;
	int resultado;
	
	for(i=0 ; i<exp; i++)
		resultado*=numero;
		
	return resultado;
}



/******************************************************************/
/* Função: Debug																									*/
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
/* Função: Debug_int																							*/
/******************************************************************/
Debug_int (char *s, int n)
{
#ifdef _DEBUG_2
printf ("%s: %d\n\n", s, n);
#endif
}


/******************************************************************/
/* Função: Debug_char																							*/
/******************************************************************/
Debug_char (char *s, char c)
{
#ifdef _DEBUG_2
printf ("%s: %c\n\n", s, c);
#endif
}


/******************************************************************/
/* Função: MostraSensorLed																				*/
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
/* Função: putCleanStrLCD																					*/
/******************************************************************/
/* Limpa e escreve na linha a string passada */
putCleanStrLCD (char *s, int L)
{
	setCursorLCD(L, 1);
	putStrLCD("                ");
	setCursorLCD(L, 1);
	putStrLCD(s);
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

		// Lê o valor do sensor escolhido.
    if (VerificaSensor > 0)
       Sensor[VerificaSensor-1].ValorLido = in(END_ROBOT);
		
//		if (AtualizaTempo == TRUE)
			ContInter +=1;
			
			printf("\nContInter: %d\n", ContInter);

}



/*################################################################*/
/*
     Main
 */
/*################################################################*/
main ()
{
	int tecla, i, NumDigitos, tempoTecla, d;
	int num_h, num_l;
	int ang_h, ang_l;
	int enc_h, enc_l;
	char s[TAM_MSG_TEXTO];
	int VerificaTecla;
	int QteMaxDigitos, *Parametro, ValorMaxPar, ValorMinPar;
	int ValorEncR;
	int ValorEncL;
	int ValorMediaEnc;
	int DistanciaTotal;
	int NumSensor;
	int ContadorEncoder;
	int ContadorSensor;
	int TempoGiro;
	int VelocidadeGiro;

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
	TeclaPressionada = 0;
	DesvioReto = FALSE;
	GiraRobo = FALSE;
	VerificaObstaculo = FALSE;
	AtualizaTempo = FALSE;
	ContInter = 0;
	DesviaRobo = FALSE;
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
	ContadorSensor = ESPERA_SENSOR;	
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
	Inicializa as interupções.
******************************************************************/

	// Marca o tempo das interrupções.
/*
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
*/
	// Habilita
    	enable();	

Debug("Desmacaro");
/*****************************************************************/

/******************************************************************
	Inicializa o Enconder.
******************************************************************/
	// Espera um tempo entre um comando e outro.
	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
	// Inicializa os Encoders.
	out(END_ROBOT, END_RESET_ENC_ALL);
/*****************************************************************/

Debug("Inicio Encoder");

/******************************************************************
		Loop Principal.
******************************************************************/
	while (TRUE)
	{
	/*****************************************************************
			Verifica se há alguma tecla pressionada.
	*****************************************************************/
	// Pega ultima tecla pressionada.
	// Espera um tempo entre um comando e outro.
	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);				
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
			case VALOR_TECLA_7:// Tecla para avançar um nivel.
				MenuNivel +=1;
				TeclaPressionada = 0;				
				break;
			case VALOR_TECLA_8:// Tecla para mudar de modo.
				if (MenuNivel == 1)
					MenuModo +=1;
				TeclaPressionada = 0;				
				break;
		}// Switch.
	/******************************************************************/

Debug("Verifico Tecla");

	/*****************************************************************
			Menu do programa.
	*****************************************************************/

	/* Verificação Modo mínimo */
	if (MenuModo <= START_NIVEL)
		{
			MenuModo = STOP;
		}

	/* Verificação Modo máximo */
	if (MenuModo > RESET_MODO)
		{
			MenuModo = MOVE_MM;
		}

	/* Verificação Nivel mínimo */
	if (MenuNivel <= START_NIVEL)
		{
			MenuNivel = NIVEL_INICIAL;
		}

	/* Voltou ao Menu inicializa as váriaveis dos parâmetros*/
	if (MenuNivel == NIVEL_INICIAL)
		{
			Velocidade = 0;
			Distancia = 0;
			Graus = 0;
			Encoder = 0;			
			VerificaSensor = 0;
		}


	/*****************************************************************
			Nível 1.
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
//			putCleanStrLCD("(Coord Iniciais)", 2);
			putCleanStrLCD("(-)", 2);
		}

	/* Modo STOP */
	if ( (MenuModo == STOP) && (MenuNivel == 1) && (tecla != 0))
		{
			putCleanStrLCD("STOP", 1);
			putCleanStrLCD("(-)", 2);
		}


	/*****************************************************************
			Nível 2.
	*****************************************************************/

/******************************************************************/
  /* Atualiza Sensores */
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
     		
    		// Lê o valor do sensor escolhido.
        if (VerificaSensor >= 0)
           Sensor[VerificaSensor].ValorLido = in(END_ROBOT);
    		
      	if (AtualizaTempo == TRUE)
    			ContInter +=1;
  		
        ContadorSensor +=1;
        
        }
     }

/******************************************************************/
	/* Modo STOP */
/******************************************************************/	
	if ( (MenuModo == STOP) && (MenuNivel == 2) && (tecla != 0))
		{
		/* Verificação Nivel máximo */
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

	Debug("Nivel 2: Passou do Stop");

/******************************************************************/
	/* Modo TESTE_SENSORES */
/******************************************************************/	
	if ( (MenuModo == TESTE_SENSORES) && (MenuNivel == 2) )
		{
		/* Verificação Nivel máximo */
		if (MenuNivel >= 2)
			{
				MenuNivel = 2;
			}        

			/* Variaveis para verificações e atualizações */
			QteMaxDigitos = 1;
			Parametro = &VerificaSensor;
			ValorMaxPar = VALOR_MAX_SENSOR;
			ValorMinPar = VALOR_MIN_SENSOR;

     if (tecla !=0)
     {
    		sprintf(s, "Teste Sensor %d", VerificaSensor);
    		putCleanStrLCD(s, 1);
    		putCleanStrLCD("<- Retorne ", 2);
     }
     
		 // _DEBUG_2
		 sprintf(s, "Valor Sensor %d: ", VerificaSensor);
		 Debug_int(s, Sensor[(VerificaSensor-1)].ValorLido);

			MostraSensorLed (Sensor[(VerificaSensor-1)].ValorLido);
		}

	Debug("Nivel 2: Teste dos Sensores");

/******************************************************************/
	/* Velocidade */
/******************************************************************/	
	if ( ( (MenuModo == MOVE_MM)     || (MenuModo == MOVE_SPD) || 
			   (MenuModo == TURN_SPD)    || (MenuModo == TURN_DEG) || 
			   (MenuModo == L_WHEEL_SPD) || (MenuModo == R_WHEEL_SPD) ) &&
		     (MenuNivel == 2)
		 ) 
		{
			/* Variaveis para verificações e atualizações */
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

	Debug("Nivel 2: Pega Velocidade");


/******************************************************************/
	/* Modo MAIN_AUTO 
		 Execução.
	*/
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

			DesvioReto = TRUE;

			//  Habilita a atualização do contador.
			AtualizaTempo = TRUE;
		}

	Debug("Nivel 2: Auto");		


	/*****************************************************************
			Tempo Limite
	*****************************************************************/
/*	if ((MenuModo == MAIN_AUTO) && (MenuNivel >= 2) && 
		  (AtualizaTempo == TRUE)
		 )
	{
		//  Tempo limite está em segundos.
		if ((ContInter >= TEMPO_LIMITE) && (DesviaRobo == FALSE))
		{
			VelocidadeGiro = VEL_GIRO_ANT_HOR;

			// Espera um tempo entre um comando e outro.
			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
			out(END_ROBOT, TURN_SPD);

			// Espera um tempo entre um comando e outro.
			for (i=0 ; i<TEMPO_ESP_COMANDO; i++);
			out(END_ROBOT, VelocidadeGiro);
      
      TempoGiro = TEMPO_GIRO_LIMITE;
   		for (i=0 ; i<TempoGiro; i++);		
		
			DesvioReto = TRUE;
			VerificaObstaculo = FALSE;
		}
		
//		Debug_int("ContInter", ContInter);
	}
*/	
	/****************************************************************/

	Debug("Nivel 2: Atualiza Tempo");

	/*****************************************************************
			Desvio Reto.
	*****************************************************************/
	if ((MenuModo == MAIN_AUTO) && (MenuNivel >= 2) && (DesvioReto == TRUE))
	{
		// Interação com o usuário
		if (MenuNivel == 2)
		{
			putCleanStrLCD("Robô",1);
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
	
  Debug("Nivel 2: Desvio Reto");		     
	
  
	/*****************************************************************
			Verifica obstaculos
	*****************************************************************/
		 if ( (MenuModo == MAIN_AUTO) && (MenuNivel >= 2) && (VerificaObstaculo == TRUE) )
		 {
      for(NumSensor=0; NumSensor<NUM_SENSORES ; NumSensor++)
			{
					// Verifica qual é o sensor que tem obstáculo.
					if ((Sensor[NumSensor].ValorLido > VALOR_SENSOR_MINIMO))
					{
						// Verifica qual é o obstaculo.
            if (NumSensor == 0)
						{
							 VelocidadeGiro = VEL_GIRO_ANT_HOR;
							 TempoGiro = 0;
						}

            if (NumSensor == 1)
						{
    				  
							VelocidadeGiro = VEL_GIRO_ANT_HOR;

							// Espera um tempo entre um comando e outro.
							for (i=0 ; i<TEMPO_ESP_COMANDO; i++);
							// Robo para onde estiver.
							out(END_ROBOT, STOP);
              TempoGiro = TEMPO_GIRO_OBST_ESQ;
						}

            if (NumSensor == 2)
						{
							VelocidadeGiro = VEL_GIRO_ANT_HOR;

							// Espera um tempo entre um comando e outro.
							for (i=0 ; i<TEMPO_ESP_COMANDO; i++);
							// Robo para onde estiver.
							out(END_ROBOT, STOP);
              TempoGiro = TEMPO_GIRO_OBST_DIR;
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
//            DesviaRobo = TRUE;
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

	Debug("Nivel 2: Verifica Obstaculo");		

	/*****************************************************************
			GiraRobo.
	*****************************************************************/
	if ((MenuModo == MAIN_AUTO) && (MenuNivel >= 2) && (GiraRobo == TRUE))
	{

		if (MenuNivel == 2)
		{
			putCleanStrLCD("Robô",1);
			putCleanStrLCD("Desviando",2);			
		}
		
		if (TempoGiro >0)
		{
/* 

// Testar Girar ângulo.

    	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
			out(END_ROBOT, TURN_DEG);
    	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
			out(END_ROBOT, 0x00);
    	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
			out(END_ROBOT, 45);
    	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
			out(END_ROBOT, VelocidadeGiro);

*/

/* 

// Testar Girar Inteligente.

		while(DistAnt < DistAtual)
			{
				DistAnt = DistAtual;
				
				// Espera um tempo entre um comando e outro.
				for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
				out(END_ROBOT, TURN_SPD);
			
				// Espera um tempo entre um comando e outro.
				for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
				out(END_ROBOT, VelocidadeGiro);
				
				for (i=0 ; i<2000; i++);

				DistAtual = Sensor[(NumSensor-1)].ValorLido;

			}


			// Volta a posição anterior.
    	// Espera um tempo entre um comando e outro.
    	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
    	out(END_ROBOT, TURN_SPD);
    
    	// Espera um tempo entre um comando e outro.
    	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
			// Volta a posição anterior.
    	out(END_ROBOT, ((VelocidadeGiro)*(-1)));

    	for (i=0 ; i<2000; i++);	

*/

///*
    	// Espera um tempo entre um comando e outro.

    	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
    	out(END_ROBOT, TURN_SPD);
    
    	// Espera um tempo entre um comando e outro.
    	for (i=0 ; i<TEMPO_ESP_COMANDO; i++);		
    	out(END_ROBOT, VelocidadeGiro);
    	
    	for (i=0 ; i<TempoGiro; i++);		

//*/
    }

		GiraRobo = FALSE;
		DesvioReto = TRUE;
//		DesviaRobo = FALSE;
		ContInter = 0;

    MostraSensorLed (Sensor[(NumSensor-1)].ValorLido);
      
  }
	
	/****************************************************************/

	Debug("Nivel 2: Gira Robo");		
	
	/*****************************************************************
			Nível 3.
	*****************************************************************/

	/* Distância */
	if ( (MenuModo == MOVE_MM) && 
	     (MenuNivel == 3)
	   ) 
		{
  
			/* Variaveis para verificações e atualizações */
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
		
	Debug("Nivel 3: Pega Distancia");		

	/* Graus */
	if ( (MenuModo == TURN_DEG) && 
		   (MenuNivel == 3)       && 
		   (tecla != 0)
	   ) 
		{
		/* Verificação Nivel máximo */
		if (MenuNivel >= 3)
			{
				MenuNivel = 3;
			}                       
			/* Variaveis para verificações e atualizações */
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
             
			/* Verificação Nivel máximo */
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

	Debug("Nivel 3: Encoder");		

	/*****************************************************************
			Nível 4.
	*****************************************************************/
  /* Encoder */
	if ( ( (MenuModo == MOVE_MM) || (MenuModo == TURN_DEG) || (MenuModo == MAIN_AUTO)) &&
		     (MenuNivel >= 4) && (tecla != 0)
		 )
		{
      ContadorEncoder +=1;
      
			/* Verificação Nivel máximo */
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

Debug("Nivel 4: Encoder");		

	/****************************************************************/


	/*****************************************************************
			Atualização de parâmetros.
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

				// Verifica se não passou do máximo.
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

				// Verifica se não passou do mínimo.
				if ((*Parametro) <= ValorMinPar)
					(*Parametro) = ValorMinPar;

				// Desmarca o Flag.
				Diminuir = FALSE;
			}

			/* Marco o Digito */
			if ((tecla != 0) && (MenuModo != TESTE_SENSORES))
				setCursorLCD(2, ((QteMaxDigitos +2) - Digito ));
				
			if ((MenuModo == TESTE_SENSORES))
				setCursorLCD(1, 14);
				
			if ((MenuModo == STOP))
				setCursorLCD(2, 1);				
				
				
		}
	/****************************************************************/

Debug("Atualiza Parametro");		

	/*****************************************************************
			Finalizo o robo, a interrupção, os leds e o display.
	*****************************************************************/
	if (FinalizaRobo == TRUE)
	{
		// Para o Robo.
		out(END_ROBOT, STOP);

		// Desabilita Interrupções.
		disable();
		
		// Apaga os Leds.
		out (END_LEDS, END_0_LEDS_LIGADOS);
		
		while (getStatusLCD());	// Aguarda o display ser liberado.
		// Desliga o display.
		out (END_DSPL, END_DSPL_DESLIGA); 

		break;
	}
	/****************************************************************/

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

Debug("Bounce");		

	}// While Principal.
/******************************************************************/
	
}

