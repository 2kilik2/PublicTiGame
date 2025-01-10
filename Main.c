#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>
#include <time.h>

#define MAP_MAX_LINES 150 
#define MAP_MAX_LENGTH 150
#define WALK_MAP_MAX_LINES 10 
#define WALK_MAP_MAX_LENGTH 10 
#define HELP_MAX_LINES 50 
#define HELP_MAX_LENGTH 200
#define RENDER_SIZE_X 120 
#define RENDER_SIZE_Y 20 
#define INPUT_MAX_LENGTH 10
#define TAX_AMOUNT 100
#define TAX_FREQUENCY 4
#define DAILY_PAYMENT 20
#define DAILY_STAMINA 10

//Granice pomocy
#define HELP_MOVE_START 0
#define HELP_MOVE_LINE_COUNT 9
#define HELP_BJ_START 10
#define HELP_BJ_LINE_COUNT 19
#define HELP_ROULETTE_START 31
#define HELP_ROULETTE_LINE_COUNT 13

//===============
// ZMIENNE OGOLNE
// ==============

int stamina = DAILY_STAMINA;
int luck;
int money = 100;
int nextTax = TAX_FREQUENCY;

//==========================
//ZMIENNE DO GRAFIKI I RUCHU
//==========================
//Zmienne mapy
char map[MAP_MAX_LINES][MAP_MAX_LENGTH];
char mapFileName[] = "map.txt";
FILE* mapFile;
char renderArea[RENDER_SIZE_Y][RENDER_SIZE_X];

//zmienne mapy chodzenia
char walkMap[WALK_MAP_MAX_LINES][WALK_MAP_MAX_LENGTH];
char walkMapFileName[] = "walkMap.txt";
FILE* walkMapFile;
int walkMapPointer[2] = { 1, 1 };

//zmienne pomocy
char helpText[HELP_MAX_LINES][HELP_MAX_LENGTH];
char helpTextFileName[] = "helpFile.txt";
FILE* helpTextFile;

//inne
char carModel[4][23] = { "7\_\/\"\;\"\"\\\_\_\_\,", "[\(\)\=\"\-\-\^\(\)\=\]", "\,\_\_\_\/\"\"\;\"\\\_f", "[\=\(\)\^\-\-\"\=\(\)\]" };
int lineCount = 0;
int keyboardInput;
int lastMove = 1; //(1 - prawo, 2 - lewo, 3 - góra, 4 - dó³)
char location;

//==============
//ZMIENNE DO GRY
//==============
//Black Jack
int blackjackDeck[13] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, };
int blackjackAces = 0;
int blackjackSum = 0;
int bet;
char tempBet[INPUT_MAX_LENGTH];
char* betCheckPointer;
char bjLastCardInfo[2];
int dealerPoints;

//=======
//PROGRAM
//=======

//MAIN ================
int main()
{
	srand(time(NULL));

	if (setupFiles())
	{
		return 1;
	}

	do
	{
		switch (move())
		{
		case 1:
			return 1;
			break;
		case 'K':
			church();
			break;
		case'D':
			house();
			break;
		case 'C':
			if (stamina <= 0)
			{
				printf("Masz za malo energii aby grac w kasynie, jedz do domu sie wyspac!\n");
				printf("\nNacisnij cokolwiek aby kontynuowac.\n");
				_getch();
			}
			else
			{
				casino();
			}
			break;
		}
	} while (true);
	return 0;
}

//Przygotowanie plikow
int setupFiles()
{
	fopen_s(&mapFile, mapFileName, "r");
	fopen_s(&walkMapFile, walkMapFileName, "r");
	fopen_s(&helpTextFile, helpTextFileName, "r");

	if (mapFile == NULL) {
		printf("Nie udalo sie otworzyc pliku mapy.\n");
		return 1;
	}
	if (walkMapFile == NULL) {
		printf("Nie udalo sie otworzyc pliku mapy chodzenia.\n");
		return 1;
	}
	if (helpText == NULL) {
		printf("Nie udalo sie otworzyc pliku mapy chodzenia.\n");
		return 1;
	}

	// Wczytywanie plikow do tablic
	while (fgets(map[lineCount], MAP_MAX_LENGTH, mapFile)) {
		map[lineCount][strcspn(map[lineCount], "\n")] = '\0'; // Usuwanie znaku nowej linii i zastepowanie go znakiem konca linii
		lineCount++;

		if (lineCount >= MAP_MAX_LINES) {
			break;
		}
	}

	lineCount = 0;
	while (fgets(walkMap[lineCount], WALK_MAP_MAX_LENGTH, walkMapFile)) {
		walkMap[lineCount][strcspn(walkMap[lineCount], "\n")] = '\0'; // Usuwanie znaku nowej linii i zastepowanie go znakiem konca linii
		lineCount++;

		if (lineCount >= WALK_MAP_MAX_LINES) {
			break;
		}
	}

	lineCount = 0;
	while (fgets(helpText[lineCount], HELP_MAX_LENGTH, helpTextFile)) {
		helpText[lineCount][strcspn(helpText[lineCount], "\n")] = '\0'; // Usuwanie znaku nowej linii i zastepowanie go znakiem konca linii
		lineCount++;

		if (lineCount >= HELP_MAX_LINES) {
			break;
		}
	}

	fclose(mapFile);
	fclose(walkMapFile);
	fclose(helpTextFile);

	return(0);
}//

int help(int start, int count)
{
	system("cls");
	for (int i = 0; i < count; i++)
	{
		printf("%s\n", helpText[i + start]);
	}
	_getch();
	system("cls");
}

//========================
//FUNKCJE DO MENU GLOWNEGO
//========================

int move()
{
	bool isDone = false;

	system("cls");
	do
	{
		renderBufor(0, 0);
		printRender();

		printf("\npomoc znajdziesz pod przyciskiem \"h\"\n");
		isDone = true;
		keyboardInput = _getch();

		system("cls");

		switch (keyboardInput)
		{
		case 224: // Któraœ strza³ka
			keyboardInput = _getch();

			switch (keyboardInput)
			{
			case 72: // Góra
				if (walkMap[walkMapPointer[0] - 1][walkMapPointer[1]] == '0')
				{
					printf("\n===\nNie mozesz pojechac w gore!! Chcesz sie rozbic?!\n===\n");
					isDone = false;
				}
				else
				{
					if (walkMap[walkMapPointer[0] - 1][walkMapPointer[1]] == '1')
					{
						walkMapPointer[0] -= 1;
						lastMove = 3;
						isDone = false;
					}
					else
					{
						return (walkMap[walkMapPointer[0] - 1][walkMapPointer[1]]);
					}
				}
				break;
			case 80: // Dó³
				if (walkMap[walkMapPointer[0] + 1][walkMapPointer[1]] == '0')
				{
					printf("\n===\nNie mozesz pojechac w dol!! Chcesz sie rozbic?!\n===\n");
					isDone = false;
				}
				else
				{
					if (walkMap[walkMapPointer[0] + 1][walkMapPointer[1]] == '1')
					{
						walkMapPointer[0] += 1;
						lastMove = 4;
						isDone = false;
					}
					else
					{
						return (walkMap[walkMapPointer[0] + 1][walkMapPointer[1] + 1]);
					}
				}
				break;
			case 77: // Prawo
				if (walkMap[walkMapPointer[0]][walkMapPointer[1] + 1] == '0')
				{
					printf("\n===\nNie mozesz pojechac w prawo!! Chcesz sie rozbic?!\n===\n");
					isDone = false;
				}
				else
				{
					if (walkMap[walkMapPointer[0]][walkMapPointer[1] + 1] == '1')
					{
						walkMapPointer[1] += 1;
						lastMove = 1;
						isDone = false;
					}
					else
					{
						return (walkMap[walkMapPointer[0]][walkMapPointer[1] + 1]);
					}
				}
				break;
			case 75: // Lewo
				if (walkMap[walkMapPointer[0]][walkMapPointer[1] - 1] == '0')
				{
					printf("\n===\nNie mozesz pojechac w lewo!! Chcesz sie rozbic?!\n===\n");
					isDone = false;
				}
				else
				{
					if (walkMap[walkMapPointer[0]][walkMapPointer[1] - 1] == '1')
					{
						walkMapPointer[1] -= 1;
						lastMove = 2;
						isDone = false;
					}
					else
					{
						return (walkMap[walkMapPointer[0]][walkMapPointer[1] - 1]);
					}
				}
				break;
			}
			break;
		case 104: // H
			help(HELP_MOVE_START, HELP_MOVE_LINE_COUNT);
			isDone = false;
			break;
		default:
			printf("Zly klawisz\n");
			isDone = false;
			break;
		}
	} while (!isDone);
	return 1;
}
int renderBufor(int x, int y)
{
	//wyrenderowanie mapy
	for (int i = 0; i < RENDER_SIZE_Y; i++)
	{
		for (int j = 0; j < RENDER_SIZE_X; j++)
		{
			renderArea[i][j] = map[i + y][j + x];
		}
	}

	//dorysowanie auta
	switch (lastMove)
	{
	case 1:
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 12; j++)
			{
				renderArea[i + 17 + ((walkMapPointer[0] - 1) * 20)][j + 14 + ((walkMapPointer[1] - 1) * 40)] = carModel[i][j];
			}
		}
		return 0;
	case 2:
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 12; j++)
			{
				renderArea[i + 15 + ((walkMapPointer[0] - 1) * 20)][j + 14 + ((walkMapPointer[1] - 1) * 40)] = carModel[i + 2][j];
			}
		}
		return 0;
	case 3:
		// Jeszcze nie obs³ugiwane
		break;
	case 4:
		// Jeszcze nie obs³ugiwane
		break;
	}
	return(1);
}
int printRender()
{
	for (int i = 0; i < RENDER_SIZE_Y; i++)
	{
		for (int j = 0; j < RENDER_SIZE_X; j++)
		{
			printf("%c", renderArea[i][j]);
		}
		printf("\n");
	}
	return 0;
}

//===============
//FUNKCJE DO DOMU
//===============

int house()
{
	if (stamina <= 0)
	{
		stamina = 10;
		printf("Wszedles do domu zmeczony, od razu poszedles sie polozyc i zasnales.\n");
		system("pause");
		system("cls");
		printf("Jest juz rano. Wyspales sie i masz 10 jednostek energii.\n\n");
		if (nextTax <= 0)
		{
			printf("Nadszedl dzien podatku, przyszedl przykry pan w plaszczu i zazadal od Ciebie $%i.\n", TAX_AMOUNT);
			if (money >= TAX_AMOUNT)
			{
				money -= TAX_AMOUNT;
				printf("Zaplaciles podatek a przykry pan sobie poszedl, wyjezdzasz na miasto swoja fura.\n");
				system("pause");
				nextTax = TAX_FREQUENCY;
			}
			else
			{
				printf("Nie stac Cie nie zaplacenie podatku, masz przy sobie zaledwie $%i\nPrzykry pan zabral Twoj dom w celu uregulowania podatku, jestes bezdomny.\n", money);
				system("pause");
				printf("PRZEGRALES!\n");
				system("pause");
				return 1;
			}
		}
		else
		{
			printf("Pamietej ze za %i dni bedziesz musial zaplacic podatki, wynosza one $100 (masz $%i), jesli ich nie zaplacisz wyladujesz na ulicy!\n", nextTax, money);
			system("pause");
			nextTax--;
		}
	}
	else
	{
		printf("Jeszcze nie jestes na tyle zmeczony zeby isc spac, Wroc tu jak bedziesz mial 0 jednostek energii.\n");
		system("pause");
	}
	return 0;
}

//=================
//FUNKCJE DO KASYNA
//=================

int casino()
{
	do {
		system("cls");
		if (stamina <= 0)
		{
			printf("Jestes juz zbyt zmeczony zeby bawic sie w kasynie. Wracaj do domu sie wyspac.");
			system("pause");
			break;
		}

		printf("Witaj w kasynie!\n");
		printf("Twoj obecny bilans konta to: $%i, masz dzis jeszcze %i jednostek energii\n\n", money, stamina);
		printf("W co chcesz zagrac:\n");
		printf("R - Ruletka\n");
		printf("B - Blackjack\n");
		printf("esc - Chce wyjsc\n");
		switch (_getch())
		{
		case 27: //ecs
			return 0;
			break;
		case 114://R
			roulette();
			break;
		case 98://B
			blackjack();
			break;
		}
	} while (true);

	return 0;
}

int blackjackIsWninned()
{
	dealerPoints = ((rand() % 10) + 11);

	if (blackjackSum > 21)
	{
		return 1;
	}
	else if (blackjackSum == 21)
	{
		return 0;
	}
	else if (blackjackSum > dealerPoints)
	{
		return 0;
	}
	else if (blackjackSum == dealerPoints)
	{
		return 3;
	}
	else
	{
		return 1;
	}
}
int blackjack()
{
	system("cls");
	printf("Witaj przy stole do gry w black jacka\n");
	printf("Wybierz ile chcesz postawic. Twoj obecny bilans konta to: %i.\n", money);
	printf("Aktywnosc ta bedzie Cie kosztowac 1 jednostke energii. Masz obecnie %i jednostek energii.\n", stamina);
	printf("Aby odejsc od stolu postaw $0.\n\n");
	printf("Twoja stawka: ");
	scanf_s("%s", tempBet, INPUT_MAX_LENGTH);
	bet = strtol(tempBet, &betCheckPointer, 10);

	while (*betCheckPointer != '\0')
	{
		printf("\nCos tu jest nie tak!\nWpisz jeszcze raz uwazajac aby wpisac tylko cyfry!\n");
		printf("Twoja stawka: ");
		scanf_s("%s", tempBet, INPUT_MAX_LENGTH);
		bet = strtol(tempBet, &betCheckPointer, 10);
	}

	if (bet == 0)
	{
		return 0;
	}

	money -= bet;
	blackjackRound();

	system("cls");
	switch (blackjackIsWninned())
	{
	case 0:
		money += bet * 2;
		printf("WYGRALES!!!\n");
		break;
	case 1:
		printf("PRZEGRALES.\n");
		break;
	case 2:
		money += bet;
		printf("REMIS.\n");
		break;
	default:
		return 1;
		break;
	}
	stamina--;
	printf("Twoj nowy bilans konta to: %i, zostalo Ci dzis jeszcze %i jednostek energii\n", money, stamina);
	system("pause");
}
int blackjackRound()
{
	blackjackAces = blackjackSum = 0;
	bjLastCardInfo[0] = '-';
	bjLastCardInfo[0] = '-';

	while (blackjackSum < 21)
	{
		system("cls");
		printf("Suma Twoich kart: %i | Ilosc Twoich asow: %i | Twoja stawka: %i | Twoj stan konta: %i | Stamina: %i\n", blackjackSum, blackjackAces, bet, money, stamina);
		printf("\npomoc znajdziesz pod przyciskiem \"h\"\n\n");
		printf("Twoja ostatnia karta to: %s\n", bjLastCardInfo);
		printf("aby dobrac karte nacisnij spacje\n");
		printf("aby spasowac nacisnij escape\n");

		switch (_getch())
		{
		case 104: //h
			help(10, 20);
			break;
		case 32: //spacja
			blackjackPickCard();
			break;
		case 27: //spacja
			return 0;
		}
	}

	for (int i = 0; i < blackjackAces; i++)
	{
		if (blackjackSum + 10 <= 21)
		{
			blackjackSum += 10;
		}
		else
		{
			break;
		}
	}
}
int blackjackPickCard()
{
	int card = blackjackDeck[rand() % 13];
	if (card < 10)
	{
		bjLastCardInfo[0] = card + 48;
		blackjackSum += card;
	}
	else if (card == 10)
	{
		bjLastCardInfo[0] = '1';
		bjLastCardInfo[1] = '0';
		blackjackSum += card;

	}
	else
	{
		switch (card)
		{
		case 11:
			bjLastCardInfo[0] = 'J';
			blackjackSum += 10;
			break;
		case 12:
			bjLastCardInfo[0] = 'Q';
			blackjackSum += 10;
			break;
		case 13:
			bjLastCardInfo[0] = 'K';
			blackjackSum += 10;
			break;
		case 14:
			bjLastCardInfo[0] = 'A';
			blackjackSum += 1;
			blackjackAces += 1;
			break;
		}
	}
}

int roulette()
{
	bool isDone = false;
	//1 zielony
	//18 czerwonych i zielonych 
	system("cls");
	printf("Witaj przy stole do gry w ruletke\n");
	printf("Wybierz ile chcesz postawic. Twoj obecny bilans konta to: %i.\n", money);
	printf("Aktywnosc ta bedzie Cie kosztowac 1 jednostke energii. Masz obecnie %i jednostek energii.\n", stamina);
	printf("Aby odejsc od stolu postaw $0.\n\n");
	printf("Twoja stawka: ");
	scanf_s("%s", tempBet, INPUT_MAX_LENGTH);
	bet = strtol(tempBet, &betCheckPointer, 10);

	while (*betCheckPointer != '\0')
	{
		printf("\nCos tu jest nie tak!\nWpisz jeszcze raz uwazajac aby wpisac tylko cyfry!\n");
		printf("Twoja stawka: ");
		scanf_s("%s", tempBet, INPUT_MAX_LENGTH);
		bet = strtol(tempBet, &betCheckPointer, 10);
	}

	if (bet == 0)
	{
		return 0;
	}

	money -= bet;

	while (!isDone)
	{

		system("cls");
		printf("Twoja stawka: $%i | Twoj stan konta: $%i Twoja energia: %i jednostek\n\n", bet, money, stamina);
		printf("Pomoc znajdziesz pod przycieskiem h\n");
		printf("Aby postawic na czarne nacisnij [B]lack\n");
		printf("Aby postawic na czerwone nacisnij [R]ed\n");
		printf("Aby postawic na zielone nacisnij [G]reen\n");

		switch (_getch())
		{
		case 104: //h
			help(HELP_ROULETTE_START, HELP_ROULETTE_LINE_COUNT);
			break;
		case 98: //b
			if ((rand() % 37) < 18)
			{
				money += bet * 2;
				system("cls");
				printf("WYGRALES!!!");
			}
			else
			{
				system("cls");
				printf("PRZEGRALES.");
			}
			isDone = true;
			break;
		case 114: //b
			if ((rand() % 37) < 18)
			{
				money += bet * 2;
				system("cls");
				printf("WYGRALES!!!");
			}
			else
			{
				system("cls");
				printf("PRZEGRALES.");
			}
			isDone = true;
			break;
		case 103://b
			if ((rand() % 37) == 0)
			{
				money += bet * 37;
				system("cls");
				printf("WYGRALES!!!");
			}
			else
			{
				system("cls");
				printf("PRZEGRALES.");
			}
			isDone = true;
			break;
		default:
			break;
		}
	}

	stamina--;
	printf("\nTwoj nowy bilans konta to: %i, zostalo Ci dzis jeszcze %i jednostek energii\n", money, stamina);
	system("pause");

	return 0;
}

//===================
//FUNKCJE DO KOSCIOLA
//===================

int church()
{
	system("cls");
	if (stamina <= 6)
	{
		printf("Jestes juz zbyt zmeczony zeby prowadzic msze.");
		system("pause");
		return 0;
	}
	while (true)
	{
		printf("Witaj w kosciele!\n");
		printf("Twoj obecny bilans konta to: $%i, masz dzis jeszcze %i jednostek energii\n\n", money, stamina);
		printf("Wybierz co chcesz zrobic:\n");
		printf("P - Pracowac\n");
		printf("esc - Wyjsc\n");
		switch (_getch())
		{
		case 27: //ecs
			return 0;
			break;
		case 112://P
			money += DAILY_PAYMENT;
			stamina -= 6;
			system("cls");
			printf("Poprowadziles dzis %i mszy, na tacy znalazles dla siebie lacznie $%i\n", (rand() % 4) + 2, DAILY_PAYMENT);
			printf("Masz teraz lacznie $%i, i %i jednostek energii\n", money, stamina);
			system("pause");
			return 0;
			break;
		}
	}
}