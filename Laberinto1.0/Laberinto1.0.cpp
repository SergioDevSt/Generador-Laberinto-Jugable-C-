#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <iostream>
#include <stack>
#include <stdlib.h> 
#include <conio.h>
#include <string>
#include <windows.h>
#include<stdio.h>
#include<string.h>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace std; //Librerias estandar 
#include "olcConsoleGameEngine.h"  //Libreria especial para el desarrollo de juegos en c++

struct jugadores {
	char nombre[30], contra[30];
	int winf, losef,winm,losem,wind,losed;
};

bool inicioUsu = false;
bool inicioSin = true;
char contrainput[30];
int opcion = NULL;
FILE* ap;
int op;
char Usu[20], Con[20];
jugadores J;

int dif;

int  LargoLaberinto;
int  AlturaLaberinto;		//Declaramos que tipo de variables vamos a usar y de que tipo para evitar problemas

class Laberinto : public olcConsoleGameEngine  //Nombre de la clase que vamos a usar
{
public:
	Laberinto()		//Llamamos la clase que hereda todos los comandos de la libreria entre ello el nombre que le pondremos a la ventana 
	{
		m_sAppName = L"Laberinto Progra"; //Definimos el nombre de la ventana
	}

private:  //Privada no queremos lo modifiquen en el main o alguna otra parte solo esta clase 
	int* PunteroPlanoC;			//Puntero para usar un array dinamico <-- Vease que es un array dinamico 

	int yp1; 
	int xp1; //Variables del jugador donde iniciara

	int xreinicio; 
	int yreinicio; //Gurdara la posicion inicial del jugador 

	int xfinal;
	int yfinal;		//Coordenadas Meta

	//Esta clase nos ayuda a ser como guias es decir que la expresion solo puede estar 1 vez. Vease esta documentacion para entender mejor https://docs.microsoft.com/en-us/dotnet/api/system.flagsattribute?view=net-6.0
	//Otro link para flags https://stackoverflow.com/questions/1448396/how-to-use-enums-as-flags-in-c
	enum //Al declararlas enum se vuelven "constexpr" vease la documentacion c++ 
	{
		CELLNORTE = 1,  //Valores que usaremos para determinar que camino va a tomar la cell o cudrado
		CELLESTE = 2,
		CELLSUR = 4,
		CELLOESTE = 8,
		CELLVISITADA = 16,
		CELLFINAL = 18,
	};

	int  CeldasVisitadas;			//Saber cuantas cells o cudrados hemos visitado del laberinto
	stack<pair<int, int>> m_stack;	//stack es un tipo de variable que facilita el algoritmo LIFO, tambien se podria un array,vector,etc.
	
	int LargoCelda;				//El tamano de los cuadrados dentro de la consola

protected:
	virtual bool OnUserCreate()
	{
		PunteroPlanoC = new int[LargoLaberinto * AlturaLaberinto]; //Declaramos un array dinamico
		memset(PunteroPlanoC, 0, LargoLaberinto * AlturaLaberinto * sizeof(int)); //Formula multiplicas la altura y el largo de la ventana por el tamano de int en bytes //Iniciamos el array dinamico con el tamano en bytes que va a ocupar FIJANDOLO porque al ser dinamico no conservara todo los valores que podriamos usar mas adelante
		
		srand(clock());		//MUY IMPORTANTE ESTA WEA ME LLEVO un buen entenderle. Basicamente hace que funcione el random bien. https://programmerclick.com/article/19581679376/
		//META
		xfinal = (LargoLaberinto - 2) + rand() % ((LargoLaberinto - 1)+ 1 - (LargoLaberinto - 2));  //variable = limite_inferior + rand() % (limite_superior +1 - limite_inferior);
		yfinal = rand() % AlturaLaberinto; //Estas son las variables de la meta
		//JUGADOR
		yp1 = rand() % AlturaLaberinto;
		xp1 = rand() % 3; //Definimos una posicion al azar x,y para nuestro jugador dependiendo las columnas y filas

		xreinicio = xp1;
		yreinicio = yp1; //Guardamos las posiciones iniciales del jugador
		//Inicio algoritmo
		int x = rand() % LargoLaberinto;  //Le damos valores aletorios a las x y y para iniciar aletorio
		int y = rand() % AlturaLaberinto;

		m_stack.push(make_pair(x, y));  //Guardamos las cordenas y se convierte el nuevo inicio de nuestro stack e inicio del programa (Vease el algoritmo stack) asi como el inicio del juego
		PunteroPlanoC[y * LargoLaberinto + x] = CELLVISITADA; //Le decimos al algoritmo que ya visitamos la celda con esta formula https://drive.google.com/file/d/10u0fwcOk_Zojh9IhUqfTx5G7jOzigAH9/view?usp=sharing
		CeldasVisitadas = 1;			//Cuantas celdas hemos visitado 
		
		
		LargoCelda = 3;				//El tamano de nuestro cuadrado
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{

		// Es una funcion lambda esto quiere decir que se genera mientras el programa corre, lo que hace tenga retroalimentacion para el uso de la funcion en tiempo real
		auto SiguienteBloque = [&](int x, int y) //Indicamos que se va a usar como puntero con la declaracion [&] en este caso de nuestro array dinamico*/
		{
			return (m_stack.top().second + y) * LargoLaberinto + (m_stack.top().first + x);  //La misma formula de siempre pero usas diferentes x / y para determinar el siguiente bloque https://drive.google.com/file/d/1EDSLo1Cd9RNViiMdwFYAThsf6jVhT6zr/view?usp=sharing
		};
		//Otro lambda con la misma formula pero ayuda mucho a la lectura
		auto posip1 = [&](int x, int y)
		{
			return (yp1 + y) * LargoLaberinto + (xp1 + x);
		};

		// Limpia la consola y la rellena de ESPACIOS
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
		//For para dibujar los cuadrados del laberinto
		for (int x = 0; x < LargoLaberinto; x++) //Inicia en 0
		{
			for (int y = 0; y < AlturaLaberinto; y++)  //Se rellena toda la columna Y primero es decir la x valdra 0 hasta que termine la primer columna y asi sucesivamente
			{
				for (int py = 0; py < LargoCelda; py++)
				{
					for (int px = 0; px < LargoCelda; px++)

						if (PunteroPlanoC[y * LargoLaberinto + x] & CELLVISITADA) // Hacemos uso de "flags" (vease enum)  y nos dice donde empezar https://drive.google.com/file/d/10u0fwcOk_Zojh9IhUqfTx5G7jOzigAH9/view?usp=sharing
							/*Paso 1
							x* (m_nPathWidth + 1) + px
							Draw(x   // <-- que vale 0 por el PRIMER IF y se mantendra asi hasta que px sea menor a LargoCelda(3) Osea nunca pasara del 2
							//Paso 2
							* (LargoCelda+ 1) // <--Este siempre valdra cuatro la imagen lo explica https://drive.google.com/file/d/1LRG11WN8tC5tmqSYOoeE8EX8oTGziYvx/view?usp=sharing
							//Paso 3
							+ px  // <-- Este determina la posicion del draw sumando lo anterior.Esto se ve en el  4 Y ULTIMO IF Cada ciclo el primero vale 0, el segundo 1, el tercero 2 y termina
							//Paso 4
							, y * (LargoCelda + 1)  // <-- Lo mismo de arriba
							//Paso 5
							+ py, // <-- py valdra 0 la primera vez y HASTA QUE TERMINE px osea el ULTIMO IF y de 2
							//Explicacion Bucle ULTIMO IF
							Ya que px se tiene que cumplir hasta que sea menor a LargoCelda Decalarada arriba (3) tendra que tomar primero el valor 0 termina el primer ciclo, el valor 1 termina el 2 segundo ciclo, el valor 2 terminar el tercer ciclo porque el siguiente daria 3 y no se cumpliria la condicion
							Esto quiere decir que rellenara de la siguiente manera 0,0 -> 1,0 -> 2,0
							//Explicacion Bucle
							Y repitira el 3 IF y tendra que volver por el paso 1-3 ahora py valiendo 1
							Por lo que rellenerias asi 0,1 - 1,1 - 2,1
							Y nuevamente  repitira el 3 IF y tendra que volver por el paso 1-3 ahora py valiendo 2
							Por lo que rellenerias asi 0,2 - 1,2 - 2,2
							PIXEL_SOLID, <-- Le decimo vamos a rellenar ese cuadro de un color solido
							FG_WHITE); <-- El colo solido es blanco
							IMPORTANTE EMPIEZA EN (0,0) LAS X,Y https://drive.google.com/file/d/1LRG11WN8tC5tmqSYOoeE8EX8oTGziYvx/view?usp=sharing
							*/
							Draw(x * (LargoCelda + 1) + px, y * (LargoCelda + 1) + py, PIXEL_SOLID, FG_WHITE);	//Cambiar a blanco esta en modo demostracion 
						else
							Draw(x * (LargoCelda + 1) + px, y * (LargoCelda + 1) + py, PIXEL_SOLID, FG_BLUE);
				}
				for (int p = 0; p < LargoCelda; p++)
				{
					//Esto borra una pared durante su trayecto solo si es necesario solo sur y este porque se complementan.
					if (PunteroPlanoC[y * LargoLaberinto + x] & CELLSUR) //Nos dice si la cell/cuadro actual tiene camino al sur. En caso afirmativo dibuja nada osea blanco (Se puede ver mas claro si se declara color solido)
						Draw(x * (LargoCelda + 1) + p, y * (LargoCelda + 1) + LargoCelda); // Borra pared sur

					if (PunteroPlanoC[y * LargoLaberinto + x] & CELLESTE) //Nos dice si la cell/cuadro actual tiene camino al Este. En caso afirmativo dibuja nada osea blanco (Se puede ver mas claro si se declara color solido)
						Draw(x * (LargoCelda + 1) + LargoCelda, y * (LargoCelda + 1) + p); // Borra pared este
				}
			}
		}

		//Genera el laberinto/algoritmo
		if (CeldasVisitadas < LargoLaberinto * AlturaLaberinto) //Evalua si debe seguir generando laberinto solo ni no ha visitado todas las cell/cuadrados
		{
			vector<int> neighbours;
			neighbours.clear();
			// Norte
			if (/*1*/m_stack.top().second > 0 && /*2*/(PunteroPlanoC[SiguienteBloque(0, -1)] & CELLVISITADA) == 0)
				//Pregunta 1 esta cell/cuadrado coincide con un limite? evaluamos su x o y dependiendo el caso y si supera los limites no le damos un valor y significa que NO es una opcion en el switch
				//Pregunta 2 esta cell/cuadrado tiene una flag de que ya esta visitada? En caso negativo (0) es decir no se ha visitado guardamos esto en un vector y se vuelve una opcion en el switch,
				//Caso contrario es decirl positivo = 1 No es una opcion en el switch.
				//Y mismo proceso para este,sur y oeste. https://drive.google.com/file/d/1EDSLo1Cd9RNViiMdwFYAThsf6jVhT6zr/view
				neighbours.push_back(0);
			// Este
			if (/*False al final*/m_stack.top().first < LargoLaberinto - 1 && (PunteroPlanoC[SiguienteBloque(1, 0)] & CELLVISITADA) == 0)
				neighbours.push_back(1);
			// Sur
			if (m_stack.top().second < AlturaLaberinto - 1 && (PunteroPlanoC[SiguienteBloque(0, 1)] & CELLVISITADA) == 0)
				neighbours.push_back(2);
			// Oeste
			if (m_stack.top().first > 0 && (PunteroPlanoC[SiguienteBloque(-1, 0)] & CELLVISITADA) == 0)
				neighbours.push_back(3);

			//Ahora en caso de que si haya celdas/cudrados disponibles hacemos esto
			if (!neighbours.empty())
			{
				srand(clock());		//MUY IMPORTANTE ESTA WEA ME LLEVO 5 hrs entenderle
				//Elige un cell/cuadro aletorio de los disponibles
				int next_cell_dir = neighbours[rand() % neighbours.size()];

				// Esto creara un camino entre la cell/cuadrado actual y el siguiente por medio de flags
				switch (next_cell_dir)
				{
				case 0: //Elige el caso 0 (Norte) es decir tiene camino al norte
					PunteroPlanoC[SiguienteBloque(0, -1)] |= CELLSUR;  //Ponemos la flag de que la celda norte a nosotros tiene camino disponible al sur (vease https://drive.google.com/file/d/1EDSLo1Cd9RNViiMdwFYAThsf6jVhT6zr/view)
					PunteroPlanoC[SiguienteBloque(0, 0)] |= CELLNORTE;  //Ponemos que nuestra celda actual tiene camino al norte disponible
					m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second - 1))); //Generamos el nuevo tope de nuestro stack con nuestra cordenas nuevas es decir norte.
					break;  //Rompemos todo este if 

				case 1: // Este
					PunteroPlanoC[SiguienteBloque(+1, 0)] |= CELLOESTE;
					PunteroPlanoC[SiguienteBloque(0, 0)] |= CELLESTE;
					m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
					break;
				case 2: // Sur
					PunteroPlanoC[SiguienteBloque(0, +1)] |= CELLNORTE;
					PunteroPlanoC[SiguienteBloque(0, 0)] |= CELLSUR;
					m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
					break;

				case 3: // Oeste
					PunteroPlanoC[SiguienteBloque(-1, 0)] |= CELLESTE;
					PunteroPlanoC[SiguienteBloque(0, 0)] |= CELLOESTE;
					m_stack.push(make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
					break;

				}
				PunteroPlanoC[SiguienteBloque(0, 0)] |= CELLVISITADA;
				CeldasVisitadas++;
			}
			else //Si no hay celdas/cuadrados disponibles regresamos 1 lugar en coordenas dentro del stack (Vease algoritmo LIFO) hasta llegar a una celda con vecinos disponibles
			{
				m_stack.pop();
			}
		}

		int sp = 1;
		//Norte
		if (GetKey(38).bPressed && (PunteroPlanoC[posip1(0, 0)] & CELLNORTE) == 1 ||  (PunteroPlanoC[posip1(1, -1)] & CELLOESTE)  == 1) { //Determinamos si puede moverse asi al norte con el uso de flags
			yp1 -= sp ;
		}
		//Sur
		if (GetKey(40).bPressed && (PunteroPlanoC[posip1(0, 0)] & CELLSUR) || (PunteroPlanoC[posip1(1, 1)] & CELLOESTE) == 1) { //Determinamos si puede moverse asi el sur con el uso de flags ademas de apoyarnos de otro cuadro https://drive.google.com/file/d/1uCX87TdMjKkrFMD4U_ncKoqf4DnKa9Hl/view?usp=sharing
			yp1 +=  sp ;			//Pues si queremos ir sur agregamos 1 positivo esta implicito podria ser directo pero es para mas facil comprension
			//Si se les ocurre una manera de manejar mejor la velocidad del jugador me avisan
		}//Repetir esto con todos
		//Este
		if (GetKey(39).bPressed && (PunteroPlanoC[posip1(0, 0)] & CELLESTE) || (PunteroPlanoC[posip1(1, 0)] & CELLOESTE) == 1) {
			xp1 +=  sp ;
		}
		//Oeste
		if (GetKey(37).bPressed && (PunteroPlanoC[posip1(0, 0)] & CELLOESTE) || (PunteroPlanoC[posip1(-1, 0)] & CELLESTE) == 1) {
			
			xp1 -=   sp;
		}
		if (GetKey(27).bPressed) { //Si damos esc volvemos a la posicion inicial 
			if (dif == 1 && inicioUsu == true) {
				J.losef += 1;
				ap = fopen(Usu, "w");
				fprintf(ap, "%s %s %d %d %d %d %d %d", J.nombre, J.contra, J.winf, J.losef, J.winm, J.losem, J.wind, J.losed);
				fclose(ap);
			}
			else if (dif == 2 && inicioUsu == true) {
				J.losem += 1;
				ap = fopen(Usu, "w");
				fprintf(ap, "%s %s %d %d %d %d %d %d", J.nombre, J.contra, J.winf, J.losef, J.winm, J.losem, J.wind, J.losed);
				fclose(ap);
			}
			else if (dif == 3 && inicioUsu == true) {
				J.losed += 1;
				ap = fopen(Usu, "w");
				fprintf(ap, "%s %s %d %d %d %d %d %d", J.nombre, J.contra, J.winf, J.losef, J.winm, J.losem, J.wind, J.losed);
				fclose(ap);
			}
			xp1 = xreinicio;
			yp1 = yreinicio;
		}

		for (int py = 0; py < LargoCelda; py++)
			for (int px = 0; px < LargoCelda; px++)
				Draw(xp1 * (LargoCelda +1 ) + px, yp1 * (LargoCelda + 1) + py, PIXEL_SOLID, FG_DARK_GREEN); // Dibujamos a nuestro jugador podria ser un objeto pero ya me canse

		stack<pair<int, int>> fin;  //Aqui declaro la salida podria unirlo con el personaje pero me canse 

		for (int py = 0; py < LargoCelda; py++) {
			for (int px = 0; px < LargoCelda; px++) {
				Draw(xfinal * (LargoCelda + 1) + px, yfinal * (LargoCelda + 1) + py, PIXEL_SOLID, FG_RED); // Dibujamos la meta con un random que lo pone casi al final del eje x
			}
			if (py >= 2) {
				fin.push(make_pair(xfinal, yfinal)); //Una vez dibujamos la meta tmb guardamos sus coordenadas en un stack dinamico
			}
		}

		if (fin.top().first == xp1 && fin.top().second == yp1) { //Al ser dinamicos podemos hacer uso de el incluso sin haber declarado un valor por ello comparamos las coordenadas de nuestro personaje (x,Y)
			if (dif == 1 && inicioUsu == true) {
				J.winf += 1;
				ap = fopen(Usu, "w");
				fprintf(ap, "%s %s %d %d %d %d %d %d", J.nombre, J.contra, J.winf, J.losef, J.winm, J.losem, J.wind, J.losed);
				fclose(ap);
				LargoLaberinto = 10 + rand() % (20 + 1 - 10);  //variable = limite_inferior + rand() % (limite_superior +1 - limite_inferior);
				AlturaLaberinto = 7 + rand() % (14 + 1 - 7);
			}
			else if (dif == 2 && inicioUsu == true ) {
				J.winm += 1;
				ap = fopen(Usu, "w");
				fprintf(ap, "%s %s %d %d %d %d %d %d", J.nombre, J.contra, J.winf, J.losef, J.winm, J.losem, J.wind, J.losed);
				fclose(ap);
				LargoLaberinto = 20 + rand() % (30 + 1 - 20);
				AlturaLaberinto = 15 + rand() % (20 + 1 - 15);
			}
			else if (dif == 3 && inicioUsu == true) {
				J.wind += 1;
				ap = fopen(Usu, "w");
				fprintf(ap, "%s %s %d %d %d %d %d %d", J.nombre, J.contra, J.winf, J.losef, J.winm, J.losem, J.wind, J.losed);
				fclose(ap);
				LargoLaberinto = 30 + rand() % (40 + 1 - 30);
				AlturaLaberinto = 20 + rand() % (25 + 1 - 20);
			}
			//Si es asi regresa al inicio aqui iria la segunda parte donde pasa de nvl,etc.
			Laberinto game1;  //Creamos nuestro objeto
			game1.ConstructConsole(160, 100, 8, 8);//Definimos el tamaño de la ventana y el tamaño de los pxls dentro de la misma en este caso de 8*8 (Si todo sale bien devuelve true)
			game1.Start(); //Iniciamos la ventana

		}
		return true;
	}
};
int main(int argc, char* argv[])
{
	do {

		cout << "Bienvenido al juego de laberinto infinito " << endl << "Donde cada intento es una nueva aventura" << endl;
		cout << "Elige la opcion que deses" << endl;
		cout << "1.- Inicia Sesion" << endl << "2.- Registrate en infinite maze " << endl << "3.- Controles" << endl << "4.- Empezar a jugar" << endl;
		cin >> op;
		switch (op) {
		case 1:
			system("CLS");
			cout << "Ingresa tu nombre de usuario: ";
			cin.ignore();
			cin.getline(Usu, 20 , '\n');
			strcat(Usu, ".txt");
			ap = fopen(Usu, "r");
			fscanf(ap, "%s %s %d %d %d %d %d %d", &J.nombre, &J.contra, &J.winf, &J.losef, &J.winm, &J.losem, &J.wind, &J.losed);
			if (ap == NULL) {
				system("CLS");
				cout << endl << "Debes registarte en maze infinity para obtener tus estadisticas" << endl << endl << endl << endl << endl  ;
				break;
			}
			cout << "Ingrese su contrasena: ";
			cin.getline(contrainput, 20, '\n');
			cout << contrainput << endl << J.contra << endl;
			if (strcmp(J.contra, contrainput) == 0) {
				system("CLS");
				fscanf(ap, "%s %s %d %d %d %d %d %d", J.nombre, J.contra, J.winf, J.losef, J.winm, J.losem, J.wind, J.losed);
				cout << "Secci[on iniciada" << endl << endl;
				inicioUsu = true;
				inicioSin = false;
				break;
			}
			else {
				cout << endl << "Contrasena incorrecta" << endl;
				break;
			}
			
		case 2:
			system("CLS");
			cout << "Bienvenido al registro Infinity Maze" << endl;
			cout << "Ingrese el nombre de ususario a registrar" << endl;
			cin.ignore();
			cin.getline(Usu, 20);
			strcpy(J.nombre, Usu);
			strcat(Usu, ".txt");//pega cadenas
			ap = fopen(Usu, "w");
			system("CLS");
			cout << "Registro exitoso" << endl;
			cout << "Ingrese la contrasena:  " ;
			cin.getline(J.contra, 100, '\n');
			fprintf(ap, "%s %s %d %d %d %d %d %d", J.nombre, J.contra, 0, 0, 0, 0, 0, 0);
			fclose(ap);
			system("CLS");
			cout << J.contra;
			cout << "Contrasena guardada" << endl;
			break;
		case 3:
			system("CLS");
			cout << "__________________Controles__________________" << endl;
			cout << "  ˄   El personaje se movera al norte" << endl;
			cout << "  ˃   El personaje se movera al este" << endl;
			cout << "  ˅   El personaje se movera al sur" << endl;
			cout << "  ˂   El personaje se movera al sur" << endl;
			cout << " esc  Regresara al jugador a su posici[on inicial y contara 1 derrota" << endl;
			cout << " Para ganar se necesitara llegar a la meta de color rojo " << endl << endl;
			break;
		case 4:
			system("CLS");
			cout << "No tienes una secci[on iniciada y tu progreso no se guardara recomendamos crear o iniciar seci[on" << endl;
			cout << "Es hora de empezar" << endl << "Seleccione la dificultad" << endl;
			cout << "1.- Facil" << endl;
			cout << "2.- Medio" << endl;
			cout << "3.- Dificil" << endl;
			cout << "4.- Regresar al men[u principal" << endl;
			cin.ignore();
			cin >> dif;
			if (dif == 1) {
				LargoLaberinto = 10 + rand() % (20 + 1 - 10);  //variable = limite_inferior + rand() % (limite_superior +1 - limite_inferior);
				AlturaLaberinto = 7 + rand() % (14 + 1 - 7);
			}
			else if (dif == 2) {
				LargoLaberinto = 20 + rand() % (30 + 1 - 20);
				AlturaLaberinto = 15 + rand() % (20 + 1 - 15);
			}
			else if (dif == 3) {
				LargoLaberinto = 30 + rand() % (40 + 1 - 30);
				AlturaLaberinto = 20 + rand() % (25 + 1 - 20);
			}
			else if (dif == 4) {
				cout << "Regresando al men[u principal" << endl;
				break;
			}else {
				cout << "Ingrese una opci[on valida" << endl;
				break;
			}
			Laberinto game;  //Creamos nuestro objeto
			game.ConstructConsole(160, 100, 8, 8);//Definimos el tamaño de la ventana y el tamaño de los pxls dentro de la misma en este caso de 8*8 (Si todo sale bien devuelve true)
			game.Start(); //Iniciamos la ventana
		}
	} while (inicioSin == true);
	while (inicioUsu == true){

		cout << "Bienvenido " << J.nombre << endl;
		cout << "Elige la opcion que deses" << endl;
		cout << "1.- Empezar a jugar" << endl  << "2.- Controles" << endl << "3.- Estadisticas" << endl << "4.- Cerrar secion" << endl;
		cin >> op;
		switch (op) {
		case 4:
			inicioUsu = false;
			inicioSin = true;
			fclose(ap);
			cout << "Seci[on cerrada" << endl;
			break;
		case 3:
			system("CLS");
			cout << J.nombre << " tus estadisticas son: " << endl;
			cout << endl << "Modo Facil" << endl << "Has ganado: " << J.winf << endl << "Y has perdido: " << J.losef << endl << endl;
			cout << "Modo Medio" << endl << "Has ganado: " << J.winm << endl << "Y has perdido: " << J.losem << endl << endl;
			cout << "Modo Dif[icil" << endl << "Has ganado: " << J.wind << endl << "Y has perdido: " << J.losed << endl << endl;
			break;
		case 2:
			system("CLS");
			cout << "__________________Controles__________________" << endl;
			cout << "  ↑   El personaje se movera al norte" << endl;
			cout << "  →   El personaje se movera al este" << endl;
			cout << "  ↓   El personaje se movera al sur" << endl;
			cout << "  ←   El personaje se movera al sur" << endl;
			cout << " esc  Regresara al jugador a su posici[on inicial y contara 1 derrota" << endl;
			cout << " Para ganar se necesitara llegar a la meta de color rojo " << endl << endl;
			break;
		case 1:
			system("CLS");
			cout << "Es hora de empezar " << J.nombre << endl << "Seleccione la dificultad" << endl;
			cout << "1.- Facil" << endl;
			cout << "2.- Medio" << endl;
			cout << "3.- Dificil" << endl;
			cout << "4.- Regresar al men[u principal" << endl;
			cin.ignore();
			cin >> dif;
			if (dif == 1) {
				LargoLaberinto = 10 + rand() % (20 + 1 - 10);  //variable = limite_inferior + rand() % (limite_superior +1 - limite_inferior);
				AlturaLaberinto = 7 + rand() % (14 + 1 - 7);
			}
			else if (dif == 2) {
				LargoLaberinto = 20 + rand() % (30 + 1 - 20);
				AlturaLaberinto = 15 + rand() % (20+1 - 15); 
			}
			else if (dif == 3) {
				LargoLaberinto = 30 + rand() % (40+1-30);
				AlturaLaberinto = 20 + rand() % (25+1-20);
			}
			else if (dif == 4) {
				cout << "Regresando al men[u principal" << endl;
				break;
			}else {
				system("CLS");
				cout << "Ingrese una opci[on valida" << endl << endl;
				break;
			}
			Laberinto game;  //Creamos nuestro objeto
			game.ConstructConsole(160, 100, 8, 8);//Definimos el tamaño de la ventana y el tamaño de los pxls dentro de la misma en este caso de 8*8 (Si todo sale bien devuelve true)
			game.Start(); //Iniciamos la ventana
		}

	} 
	return 0; //Informamos todo salio bien
}