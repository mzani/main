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

#include "\mc85\8085io.h"
#include "\mc85\8085int.h"

#include "C:\USERS\MZani\defines.h"

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
// Variavel responsável pelo Reposicionamento e direcionamento do Robo.
int PosicionaRobo;
// Variavel responsável pela atualização das coordenadas.
int AtualizaCoordenadas;
// Variavel responsável pelo Desvio Reto
int DesvioReto;
// Variavel responsavel pelo Giro do Robo.
int GiraRobo;

// Conjunto de Variáveis responsaveis pelos valores dos parâmetros.
int Velocidade;
int Distancia;
int Graus;
int Encoder;
int Sensor;

int ValorSensor1;
int ValorSensor2;
int ValorSensor3;
int ValorSensor4;

int ValorXAtual;
int ValorYAtual;
int ValorAnguloAtual;

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
		// Le e guarda o valor lido do sensor 1.
			out(END_ROBOT, END_RD_GP2D_1);
			ValorSensor1 = in(END_ROBOT);

		for (i=0 ; i<TEMPO_ESP_INIC; i++);
		// Le e guarda o valor lido do sensor 2.
			out(END_ROBOT, END_RD_GP2D_2);
			ValorSensor2 = in(END_ROBOT);

		for (i=0 ; i<TEMPO_ESP_INIC; i++);
		// Le e guarda o valor lido do sensor 3.
			out(END_ROBOT, END_RD_GP2D_3);
			ValorSensor3 = in(END_ROBOT);

		for (i=0 ; i<TEMPO_ESP_INIC; i++);		
		// Le e guarda o valor lido do sensor 4.
			out(END_ROBOT, END_RD_GP2D_4);
			ValorSensor4 = in(END_ROBOT);

}



/*################################################################*/
/*
     Main
 */
/*################################################################*/
main ()
{
	int tecla, i, NumDigitos, tempoTecla;
	int num_h, num_l;
	int ang_h, ang_l;
	int enc_h, enc_l;
	char s[TAM_MSG_TEXTO];
	int VerificaTecla;
  int QteMaxDigitos, *Parametro, ValorMaxPar, ValorMinPar, NumMaxDigitos;
	int ValorEncR;
	int ValorEncL;
	int ValorMediaEnc;
	int ValorSensorComp;
	int DistanciaTotal;
	int ValorFinalX;
	int ValorFinalY;

/******************************************************************
	Inicializa as Variaveis Globais.
******************************************************************/

	PosicionaRobo = FALSE;
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
	Sensor = 0;
	TeclaPressionada = 0;
	ValorAnguloAtual = 0;
	ValorFinalX = 0;
	ValorFinalY = 0;
	AtualizaCoordenadas = FALSE;
	DesvioReto = FALSE;
	GiraRobo = TRUE;
/*****************************************************************/


/******************************************************************
	Inicializa as Variaveis Locais.
******************************************************************/
	NumDigitos=0;
	tempoTecla = 0;
	VerificaTecla = TRUE;
	ValorSensorComp = 0;
	DistanciaTotal = 0;
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
	out(END_ROBOT, END_TMR_MS);
	out(END_ROBOT, TEMPO_INTERRUPCAO);
	//Inicializa o timer.
	out (END_ROBOT, END_TMR_ON);
	
	//desmascara
	  asm {
	    mvi A,$1D
	    sim
	  }
	//Habilita
    	enable();	
/*****************************************************************/

/******************************************************************
	Inicializa as interupções.
******************************************************************/
	// Inicializa os Encoders.
	out(END_ROBOT, END_RESET_ENC_ALL);

/*****************************************************************/


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


	/*****************************************************************
			Menu do programa.
	*****************************************************************/

	/* Verificação Modo mínimo */
	if (MenuModo <= START_NIVEL)
		{
			MenuModo = STOP;
		}

	/* Verificação Modo máximo */
	if (MenuModo >= RESET_MODO)
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
			Sensor = 0
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
			putCleanStrLCD("(Coord Iniciais)", 2);
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

	/* Modo STOP */
	if ( (MenuModo == STOP) && (MenuNivel == 2) && (tecla != 0))
		{
			/*Comandos Parar*/
			putCleanStrLCD("STOP Confirmado", 1);
			putCleanStrLCD("<- Retorne ", 2);

			out(END_ROBOT, STOP);
		}

	/* Modo TESTE_SENSORES */
	if ( (MenuModo == TESTE_SENSORES) && (MenuNivel == 2) && (tecla != 0))
		{
			sprintf(s, "Teste Sensor %d", Sensor);
			putCleanStrLCD(s, 1);
			putCleanStrLCD("<- Retorne ", 2);

			/* Variaveis para verificações e atualizações */
			QteMaxDigitos = 1;
			Parametro = &Sensor;
			ValorMaxPar = VALOR_MAX_SENSOR;
			ValorMinPar = VALOR_MIN_SENSOR;

			switch (Sensor)
			{
			case 1:
				MostraSensorLed (ValorSensor1);
				break;
			case 2:
				MostraSensorLed (ValorSensor2);
				break;
			case 3:
				MostraSensorLed (ValorSensor3);
				break;
			case 4:
				MostraSensorLed (ValorSensor4);
				break;
			}
		}


	/* Modo MAIN_AUTO 
		 Coordenada X
	*/
	if ((MenuModo == MAIN_AUTO) && (MenuNivel == 2) && (tecla != 0))
		{
			/* Variaveis para verificações e atualizações */
			QteMaxDigitos = 3;
			Parametro = &ValorFinalX;
			ValorMaxPar = COORD_X_MAX;
			ValorMinPar = COORD_X_MIN;

			/* Escreve no visor */
			if ((tecla != 0))
			{
				setCursorLCD(1,1);
				putCleanStrLCD("Coord Inicial X", 1);

				if (ValorFinalX >= 0)
					sprintf(s, "+%03d", ValorFinalX);
				else
					sprintf(s, "%04d", ValorFinalX);

				putCleanStrLCD(s, 2);	
			}
		}


	/* Velocidade */
	if ( ( (MenuModo == MOVE_MM)     || (MenuModo == MOVE_SPD) || 
			   (MenuModo == TURN_SPD)    || (MenuModo == TURN_DEG) || 
			   (MenuModo == L_WHEEL_SPD) ||(MenuModo == R_WHEEL_SPD) ) &&
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
				out(END_ROBOT, MenuModo);
				out(END_ROBOT, Velocidade);
			}

			/* Escreve no visor */
			if ((tecla != 0))
			{
				setCursorLCD(1,1);
				putCleanStrLCD("Velocidade", 1);

				if (Velocidade >= 0)
					sprintf(s, "+%02d", Velocidade);
				else
					sprintf(s, "%03d", Velocidade);

				putCleanStrLCD(s, 2);	
			}
		}


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
					sprintf(s, "+%05d", Distancia);
				else
					sprintf(s, "%06d", Distancia);

				putCleanStrLCD(s, 2);
			}
			
		}

	/* Graus */
	if ( (MenuModo == TURN_DEG) && 
		   (MenuNivel == 3) && (tecla != 0)
		 ) 
		{
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


	/* Encoder */
	if ( ( (MenuModo == MOVE_SPD) ||
			   (MenuModo == TURN_SPD) ||
			   (MenuModo == L_WHEEL_SPD) ||(MenuModo == R_WHEEL_SPD) ) && 
		     (MenuNivel >= 3) && (tecla != 0)
		 ) 
		{
			/* Verificação Nivel máximo */
			if (MenuNivel >= 3)
				{
					MenuNivel = 3;
				}

			setCursorLCD(1,1);

			out(END_ROBOT, END_RD_ENC_L_2B);
			enc_h = in(END_ROBOT);
			enc_l = in(END_ROBOT);
			sprintf(s, "Enc L: +%05d", (enc_h*256) + enc_l);
			putCleanStrLCD(s,1);

			out(END_ROBOT, END_RD_ENC_R_2B);
			enc_h = in(END_ROBOT);
			enc_l = in(END_ROBOT);
			sprintf(s, "Enc R: +%05d", (enc_h*256) + enc_l);
			putCleanStrLCD(s,2);
		}


	/* Modo MAIN_AUTO 
		 Coordenada Y
	*/
	if ((MenuModo == MAIN_AUTO) && (MenuNivel == 3) && (tecla != 0))
		{
			/* Variaveis para verificações e atualizações */
			QteMaxDigitos = 3;
			Parametro = &ValorFinalY;
			ValorMaxPar = COORD_Y_MAX;
			ValorMinPar = COORD_Y_MIN;

			/* Escreve no visor */
			if ((tecla != 0))
			{
				setCursorLCD(1,1);
				putCleanStrLCD("Coord Inicial Y", 1);

				if (ValorFinalY >= 0)
					sprintf(s, "+%03d", ValorFinalY);
				else
					sprintf(s, "%04d", ValorFinalY);

				putCleanStrLCD(s, 2);	
			}

			// Estabiliza para o modo automático.
			out(END_ROBOT, STOP);
		}


	/*****************************************************************
			Nível 4.
	*****************************************************************/

  /* Encoder */
	if ( ( (MenuModo == MOVE_MM)     || (MenuModo == TURN_DEG)   ) &&
		     (MenuNivel >= 4) && (tecla != 0)
		 )
		{
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
			out(END_ROBOT, MenuModo);
			out(END_ROBOT, num_h);
			out(END_ROBOT, num_l);
			out(END_ROBOT, Velocidade);

			setCursorLCD(1,1);

			out(END_ROBOT, END_RD_ENC_L_2B);
			enc_h = in(END_ROBOT);
			enc_l = in(END_ROBOT);
			sprintf(s, "Enc L: +%05d", (enc_h*256) + enc_l);
			putCleanStrLCD(s,1);

			out(END_ROBOT, END_RD_ENC_R_2B);
			enc_h = in(END_ROBOT);
			enc_l = in(END_ROBOT);
			sprintf(s, "Enc R: +%05d", (enc_h*256) + enc_l);
			putCleanStrLCD(s,2);
		}


	/* Modo MAIN_AUTO 
		 Execução.
	*/
	if ((MenuModo == MAIN_AUTO) && (MenuNivel == 4) && (tecla != 0))
		{
			setCursorLCD(1,1);
			putCleanStrLCD("Moto Automatico", 1);
			putCleanStrLCD("<- Retorne", 2);

			PosicionaRobo = TRUE;
			AtualizaCoordenadas = TRUE;
		}

	/****************************************************************/


	/*****************************************************************
			Desviar dos obstaculos e atualiza as coordenadas
	*****************************************************************/
		 if ( ((MenuModo == MAIN_AUTO) && (MenuNivel == 4))&&
				 (ValorSensor1 > VALOR_SENSOR_MINIMO) ||
				 (ValorSensor2 > VALOR_SENSOR_MINIMO) ||
				 (ValorSensor3 > VALOR_SENSOR_MINIMO) ||
				 (ValorSensor4 > VALOR_SENSOR_MINIMO)
				)
		 {

			// Lógica para efetuar curvas.
			if ((ValorSensor1 > VALOR_SENSOR_MINIMO) && (DesvioReto == FALSE))
			{
				// Robo para onde estiver.
				out(END_ROBOT, STOP);
				GiraRobo = TRUE;
			}

		 }
	/****************************************************************/

	// Desvia Robo.
	if (GiraRobo = TRUE)
	{
		if (ValorSensor2 > ValorSensorComp)
		{
			// Guarda o Valor para ser comparado.
			ValorSensorComp = ValorSensor2;
			ValorAnguloAtual +=ANG_GIRO;
		}
		else
		{
			ValorAnguloAtual -=ANG_GIRO;
			GiraRobo = FALSE;
			DesvioReto = TRUE;
		}

		// Gira em 10 graus.
		out(END_ROBOT, TURN_DEG);

		ang_h = (ValorAnguloAtual >> 8);
		ang_l = ValorAnguloAtual - (ValorAnguloAtual << 8);

		out(END_ROBOT, ang_h);
		out(END_ROBOT, ang_l);
		out(END_ROBOT, VEL_GIRO);
	}


	/*****************************************************************
			Atualização de parâmetros.
	*****************************************************************/
		if (MenuNivel >= 2)
		{
			NumMaxDigitos = (ValorMaxPar/10) + 1;
			
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
			if (tecla != 0)
				setCursorLCD(2, ((NumMaxDigitos + 1) - Digito ));
		}
	/****************************************************************/


	/******************************************************************
		Reposiciona e anda o Robo.
	******************************************************************/
	if (PosicionaRobo == TRUE)
	{
		// Calcula o Angulo (Equação da Reta).
		ValorAnguloAtual = ( (ValorFinalY-ValorAtualY)/ (ValorFinalX-ValorAtualX) );

		// Gira o angulo calculado.
		out(END_ROBOT, TURN_DEG);

		ang_h = (ANG_GIRO >> 8);
		ang_l = ANG_GIRO - (ANG_GIRO << 8);

		out(END_ROBOT, ang_h);
		out(END_ROBOT, ang_l);
		out(END_ROBOT, VEL_GIRO);


		// Calcula a Distancia a ser percorrida (Pitagorás).
		DistanciaTotal = pow( (((ValorFinalX-ValorAtualX)*(ValorFinalX-ValorAtualX)) + 
													((ValorFinalY-ValorAtualY)*(ValorFinalY-ValorAtualY))), (0.5) );

		// Percorre a distancia calculada
		out(END_ROBOT, MOVE_MM);

		num_h = (DistanciaTotal >> 8);
		num_l = DistanciaTotal - (DistanciaTotal << 8);

		out(END_ROBOT, num_h);
		out(END_ROBOT, num_l);
		out(END_ROBOT, VEL_CONT);

		PosicionaRobo = FALSE;
	}
	/*****************************************************************/


	/******************************************************************
		Atualiza as coordenadas.
	******************************************************************/
	if (AtualizaCoordenadas == TRUE)
	{
		 // Atualiza as coordenadas de acordo com os encoders
		 out(END_ROBOT, END_RD_ENC_L_2B);
		 enc_h = in(END_ROBOT);
		 enc_l = in(END_ROBOT);
		 ValorEncL = (enc_h*256) + enc_l;

		 out(END_ROBOT, END_RD_ENC_R_2B);
		 enc_h = in(END_ROBOT);
		 enc_l = in(END_ROBOT);
		 ValorEncR = (enc_h*256) + enc_l;

		 // Media dos Valores dos Encoders.
		 ValorMediaEnc = (ValorEncL + ValorEncR)/2;

			if (((ValorAnguloAtual%360) > 0) && ((ValorAnguloAtual%360) < 180))
			{
				ValorXAtual += ValorMediaEnc* cos(ValorAnguloAtual);
				ValorYAtual += ValorMediaEnc* sin(ValorAnguloAtual);
			}
			else
			{
				ValorXAtual -= ValorMediaEnc* cos(ValorAnguloAtual);
				ValorYAtual -= ValorMediaEnc* sin(ValorAnguloAtual);		
			}

			// Inicializa os Encoders.
			out(END_ROBOT, END_RESET_ENC_ALL);
	}
	/****************************************************************/


	/*****************************************************************
			Desvio Reto.
	*****************************************************************/
	if (DesvioReto == TRUE)
	{
		 if ((ValorSensor1 <= VALOR_SENSOR_MINIMO) ||
				 (ValorSensor2 <= VALOR_SENSOR_MINIMO) ||
				 (ValorSensor3 <= VALOR_SENSOR_MINIMO) ||
				 (ValorSensor4 <= VALOR_SENSOR_MINIMO)
				)
		{
			DesvioReto=FALSE;
			PosicionaRobo = TRUE;
		}
		out(END_ROBOT, MOVE_SPD);
		out(END_ROBOT, VEL_DESVIO);
	}
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
		
		// Apaga os Leds.
		out (END_LEDS, END_0_LEDS_LIGADOS);
		
		while (getStatusLCD());	// Aguarda o display ser liberado.
		// Desliga o display.
		out (END_DSPL, END_DSPL_DESLIGA); 

		break;
	}
	/****************************************************************/


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

	}// While Principal.
/******************************************************************/
	
}

