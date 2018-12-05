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

// Conjunto de Variáveis responsaveis pelos valores dos parâmetros.
int Velocidade;
int Distancia;
int Graus;
int Encoder;


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



/*################################################################*/
/*
     Main
 */
/*################################################################*/
main ()
{
	int tecla, i, NumDigitos, tempoTecla;
	int num_h, num_l;
	int enc_h, enc_l;
	char s[TAM_MSG_TEXTO];
	int VerificaTecla;

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
	TeclaPressionada = 0;
/*****************************************************************/


/******************************************************************
	Inicializa as Variaveis Locais.
******************************************************************/
	NumDigitos=0;
	tempoTecla = 0;
	VerificaTecla = TRUE;
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
		}

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

	/* Modo STOP */
	if ( (MenuModo == STOP) && (MenuNivel == 1) && (tecla != 0))
		{
			putCleanStrLCD("STOP", 1);
			putCleanStrLCD("(-)", 2);
		}

	/* Modo STOP */
	if ( (MenuModo == STOP) && (MenuNivel == 2) && (tecla != 0))
		{
			/*Comandos Parar*/
			putCleanStrLCD("STOP Confirmado", 1);
			putCleanStrLCD("<- Retorne ", 2);
			out(END_ROBOT, STOP);
		}

	/* Velocidade */
	if ( ( (MenuModo == MOVE_MM)     || (MenuModo == MOVE_SPD) || 
			   (MenuModo == TURN_SPD)    || (MenuModo == TURN_DEG) || 
			   (MenuModo == L_WHEEL_SPD) ||(MenuModo == R_WHEEL_SPD) ) &&
		     (MenuNivel == 2)
		 ) 
		{
			/* Se o Digito passar do mais significativo, ele volta ao primeiro */
			if (Digito > 1)
				Digito = 0;

			/* Aumenta o Digito selecionado*/
			if (Aumentar == TRUE)
			{
				// Soma o devido digito selecinado.
				Velocidade +=(1 * pow(10, Digito) );

				// Verifica se não passou do máximo.
				if (Velocidade >= VEL_MAX)
					Velocidade = VEL_MAX;

				// Desmarca o Flag.
				Aumentar = FALSE;
			}

			/* Diminui o Digito selecionado*/
			if (Diminuir == TRUE)
			{
				// Subtrai o devido digito selecinado.
				Velocidade -=(1 * pow(10, Digito) );

				// Verifica se não passou do mínimo.
				if (Velocidade <= VEL_MIN)
					Velocidade = VEL_MIN;

				// Desmarca o Flag.
				Diminuir = FALSE;
			}

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
			
			/* Marco o Digito */
			if (tecla != 0)
				setCursorLCD(2, (3 - Digito ));			
		}

	/* Distância */
	if ( (MenuModo == MOVE_MM) && 
			 (MenuNivel == 3)
		 ) 
		{
			/* Se o Digito passar do mais significativo, ele volta ao primeiro */
			if (Digito > 4)
				Digito = 0;

			/* Aumenta o Digito selecionado*/
			if (Aumentar == TRUE)
			{
				// Soma o devido digito selecinado.
				Distancia +=(1 * pow(10, Digito) );

				// Verifica se não passou do máximo.
				if (Distancia >= DIST_MAX)
					Distancia = DIST_MAX;

				// Desmarca o Flag.
				Aumentar = FALSE;
			}

			/* Diminui o Digito selecionado*/
			if (Diminuir == TRUE)
			{
				// Subtrai o devido digito selecinado.
				Distancia -=(1 * pow(10, Digito) );

				// Verifica se não passou do mínimo.
				if (Distancia <= DIST_MIN)
					Distancia = DIST_MIN;

				// Desmarca o Flag.
				Diminuir = FALSE;
			}

			/* Escreve no visor */
			if ((tecla != 0))
			{
				setCursorLCD(1,1);
				putCleanStrLCD("Graus", 1);
				if (Distancia >= 0)
					sprintf(s, "+%05d", Distancia);
				else
					sprintf(s, "%06d", Distancia);
				putCleanStrLCD(s, 2);
			}
			
			/* Marco o Digito */
			if (tecla != 0)
				setCursorLCD(2, (6 - Digito ));			
		}

	/* Graus */
	if ( (MenuModo == TURN_DEG) && 
		   (MenuNivel == 3) && (tecla != 0)
		 ) 
		{
			/* Se o Digito passar do mais significativo, ele volta ao primeiro */
			if (Digito > 4)
				Digito = 0;
			
			/* Aumenta o Digito selecionado*/
			if (Aumentar == TRUE)
			{
				// Soma o devido digito selecinado.
				Graus +=(1 * pow(10, Digito) );

				// Verifica se não passou do máximo.
				if (Graus >= GRAU_MAX)
					Graus = GRAU_MAX;

				// Desmarca o Flag.
				Aumentar = FALSE;
			}

			/* Diminui o Digito selecionado*/
			if (Diminuir == TRUE)
			{
				// Subtrai o devido digito selecinado.
				Graus -=(1 * pow(10, Digito) );

				// Verifica se não passou do mínimo.
				if (Graus <= GRAU_MIN)
					Graus = GRAU_MIN;

				// Desmarca o Flag.
				Diminuir = FALSE;
			}

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
			
			/* Marco o Digito */
			if (tecla != 0)
				setCursorLCD(2, (6 - Digito ));			
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

