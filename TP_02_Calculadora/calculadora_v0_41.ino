#include <Keypad.h>
#include <LiquidCrystal.h>
#include <string.h>
#define BOTON_OFF 0
#define M1 1
#define M2 2

#define FILA_OPERANDOS 0
#define COLUMNA_PRIMER_OPERANDO 0
#define COLUMNA_SEGUNDO_OPERANDO 9

#define FILA_RESULTADO 1
#define COLUMNA_RESULTADO 0

#define FILA_M 1
#define COLUMNA_M 15

#define COLUMNA_FIN 16

#define	MIN_M1 -99999
#define MAX_M1 99999

#define POS_SIGNO_OPERANDO1 0
#define POS_SIGNO_OPERANDO2 9

#define POS_OPERANDO1 1
#define POS_OPERANDO2 10

LiquidCrystal lcd(0, 1, 2, 3, 4, 13);

char operando1[6];
char operando2[6];
char operador;
double numero1;
double numero2;
double resultado;
double numeroEnM1;
int i = 0;
int flagOperando1 = 1;
int flagOperando2 = 0;
int flagOperador = 0;
int flagResultado = 0;
int flagNuevaOperacion = 0;
int flagMemoriaOcupada = 0;
int flagResultadoEnPantalla = 0;
int flagResultadoErroneo = 0;
int hayNumero = 0;
int contadorSignoNegativo = 0;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = { { '1', '2', '3', '+'},
						  { '4', '5', '6', '-'},
						  { '7','8', '9', '*' },
						  { '_', '0', '=', '/'} };

byte rowPins[ROWS] = { 12, 11, 10, 9 };
byte colPins[COLS] = { 8, 7, 6, 5 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
	lcd.begin(16, 2);
	lcd.setCursor(0, 0);
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
}

int botonAntes = 0;

void loop() {

	int botonAhora = LeerBoton();
	if (botonAhora && !botonAntes) { //M1
		if (botonAhora == M1 && !flagResultadoErroneo) {
			aplicarM1();
		}
		//M2
		else if (botonAhora == M2 && flagMemoriaOcupada) {
			aplicarM2();
		}
	}
	botonAntes = botonAhora;
	//////////////------------>INGRESO E INICIO DE OPERACIONES<------------/////////////////////////

	char key = keypad.getKey();

	if (key != NO_KEY) {
		if (flagNuevaOperacion && isdigit(key)) {
			lcdLimpiarSalvoM();

			flagNuevaOperacion = 0;
			contadorSignoNegativo = 0;
			flagResultadoEnPantalla = 0;
			flagOperando1 = 1;
		}
		//////////////------------>PRIMER OPERANDO<------------/////////////////////////
		if (flagOperando1) {
			ingresarOperandos(POS_OPERANDO1, POS_SIGNO_OPERANDO1, operando1, 1,
					key);
		}

		//////////////------------>OPERADORES (+-*/)<------------/////////////////////////
		if (!isdigit(key) && key != '=' && key != '_' && flagOperador) {
			operador = key;
			lcdImprimirCaracteres(0, 7, (String) operador);
			flagOperando1 = 0;
			flagOperando2 = 1;
			i = 0;
			contadorSignoNegativo = 0;
			hayNumero = 0;
			flagOperador = 0;
		}
		//////////////------------>SEGUNDO OPERANDO<------------/////////////////////////
		if (flagOperando2) {
			ingresarOperandos(POS_OPERANDO2, POS_SIGNO_OPERANDO2, operando2, 2,
					key);
		}

		/////////////////////////////------------>RESULTADO<------------//////////////////////////////
		if (key == '=' && flagResultado) {
			if (RealizarOperacionMatematica(operador)) {
				lcdImprimirCaracteres(FILA_RESULTADO, COLUMNA_RESULTADO, "Math error");
				flagResultadoErroneo = 1;
			} else {
				lcdLimpiar(FILA_RESULTADO, COLUMNA_RESULTADO, COLUMNA_M);
				lcdImprimirCaracteres(FILA_RESULTADO, COLUMNA_RESULTADO, "=");
				lcdImprimirResultadoEnDouble(FILA_RESULTADO, COLUMNA_RESULTADO + 2, resultado);
			}

			LimpiarArrayNumero(operando1);
			LimpiarArrayNumero(operando2);

			flagOperando2 = 0;
			flagResultado = 0;
			i = 0;
			flagNuevaOperacion = 1;
			flagResultadoEnPantalla = 1;
		}
	}

	delay(50);
}

//////////////------------>FUNCIONES<------------/////////////////////////

///  @brief
///
/// @return
int LeerBoton() //jumboCheck
{
	int lectura = BOTON_OFF;
	int boton_1 = analogRead(A0);
	int boton_2 = analogRead(A1);
	if (boton_1) {
		lectura = M1;
	} else if (boton_2) {
		lectura = M2;
	}
	return lectura;
}
/// @brief Vacia las posiciones dentro del array
///
/// @param numero es el array a recorrer.
void LimpiarArrayNumero(char numero[]) {
	for (int i = 0; i < 6; i++) {
		numero[i] = '\0';
	}
}
/// @brief Borra caracteres impresos en el LCD
///exceptuando la ultima celda de la fila 1, conservando el marcador de memoria
void lcdLimpiarSalvoM() {
	lcdLimpiar(FILA_OPERANDOS, COLUMNA_PRIMER_OPERANDO, COLUMNA_FIN);
	lcdLimpiar(FILA_RESULTADO, COLUMNA_RESULTADO, COLUMNA_M);
}
/// @brief Permite asignar las posiciones de las celdas
///del inicio al fin que seran limpiadas
/// @param fila asigna la fila a limpiar
/// @param columnaInicio asigna la primera posicion dentro de la fila
/// @param columnaFin asigna la ultima posicion dentro de la fila
void lcdLimpiar(int fila, int columnaInicio, int columnaFin) {
	for (int i = columnaInicio; i < columnaFin; i++) {
		lcd.setCursor(i, fila);
		lcd.print(' ');
	}
}
/// @brief Imprime caracteres asignando la primera posicion
/// en referencia a fila/columna
/// @param fila asigna la fila a imprimir
/// @param columna asigna la primera posicion a imprimir dentro de la fila
/// @param dato Son los caracteres a imprimir en forma de String
void lcdImprimirCaracteres(int fila, int columna, String dato) {
	lcd.setCursor(columna, fila);
	lcd.print(dato);
}
/// @brief Imprime caracteres numericos asignando la primera posicion
/// en referencia a fila/columna
/// @param fila asigna la fila a imprimir
/// @param columna asigna la primera posicion a imprimir dentro de la fila
/// @param dato Son los caracteres a imprimir en forma de numero Long
void lcdImprimirNum(int fila, int columna, long dato) {
	lcd.setCursor(columna, fila);
	lcd.print(dato);
}
/// @brief Imprime caracteres numericos asignando la primera posicion
/// en referencia a fila/columna
/// @param fila asigna la fila a imprimir
/// @param columna asigna la primera posicion a imprimir dentro de la fila
/// @param dato Son los caracteres a imprimir en forma de numero Double
void lcdImprimirResultadoEnDouble(int fila, int columna, double dato) {
	lcd.setCursor(columna, fila);
	lcd.print(dato);
}
/// @brief Recibe dos parametros numericos a sumar.
///	Asigna el valor a un puntero resultado
/// @param n1 representa el valor del primer sumando
/// @param n2 representa el valor del segundo sumando
/// @param resultado es el valor resultante de la operacion.
void SumarOperandos(double n1, double n2, double *resultado) {
	*resultado = n1 + n2;
}
/// @brief Recibe dos parametros numericos a restar.
///	Asigna el valor a un puntero resultado
/// @param n1 representa el valor del  minuendo
/// @param n2 representa el valor del  sustraendo
/// @param resultado es el valor resultante de la operacion.
void RestarOperandos(double n1, double n2, double *resultado) {
	*resultado = n1 - n2;
}
/// @brief Recibe dos parametros numericos a multiplicar.
///	Asigna el valor a un puntero resultado
/// @param n1 representa el valor del  multiplicando
/// @param n2 representa el valor del  multiplicador
/// @param resultado es el producto de la operacion.
void MultiplicarOperandos(double n1, double n2, double *resultado) {
	*resultado = n1 * n2;
}
/// @brief Recibe dos parametros numericos a dividir.
///	Asigna el valor a un puntero resultado
///  Asigna resultado del cociente de la operacion a traves de puntero.
/// @param n1 representa el valor del  dividendo
/// @param n2 representa el valor del  divisor
/// @param zerDivisionError retorna 0 si es efectivo y 1 ante falla.
int DividirPrimeroPorSegundo(double dividendo, double divisor,
		double *resultado) {
	int zeroDivisionError = 1;
	if (divisor != 0) {
		*resultado = dividendo / divisor;
		zeroDivisionError = 0;
	}
	return zeroDivisionError;
}
/// @brief Recibe por parametro el operador.
/// Asigna referente al operador la operacion pertinente a realizar
/// @param operador el operador asignado para la operacion
/// @return en caso de una operacion fallida devuelve 1
/// en lugar de asignar los valores correspondientes a la operacion
/// por puntero.
int RealizarOperacionMatematica(char operador) {
	int error = 0;
	switch (operador) {
	case '+':	//suma
		SumarOperandos(numero1, numero2, &resultado);
		break;

	case '-':	//resta
		RestarOperandos(numero1, numero2, &resultado);
		break;

	case '*':	//multiplicación
		MultiplicarOperandos(numero1, numero2, &resultado);
		break;

	case '/':	//división
		error = DividirPrimeroPorSegundo(numero1, numero2, &resultado);
		break;
	}
	return error;
}
/// @brief
///Evalua el valor registrado en el operando1/operando2/resultado
///validando o invalidando la posibilidad de guardarlo en memoria
/// para posterior uso. En caso de un guardado exitos imprime el caracter M en pantalla.
/// Caso contrario por imposibilidad presenta mensaje de error.
/// La limitacion es referente a la incapacidad de imprimir numeros de mas de 5 cifras por operando.
void aplicarM1() {
	if (resultado > MAX_M1 || resultado < MIN_M1) {
		lcdLimpiar(FILA_RESULTADO, COLUMNA_RESULTADO, COLUMNA_M);
		lcdImprimirCaracteres(FILA_RESULTADO, COLUMNA_RESULTADO, "OUT OF MEMORY");
		resultado = 0;
	} else {
		lcdImprimirCaracteres(FILA_M, COLUMNA_M, "M");

		if (flagOperando1) {
			numeroEnM1 = numero1;
			flagMemoriaOcupada = 1;
		} else if (flagOperando2) {
			numeroEnM1 = numero2;
			flagMemoriaOcupada = 1;
		} else if (flagResultadoEnPantalla) {
			numeroEnM1 = resultado;
			flagMemoriaOcupada = 1;
		}
	}
}
/// @brief Permite imprimir el valor almacenado en memoria en la posicion del primer y/o segundo operando
///dependiendo cual sea el operando activo.
///Previo a la impresion en caso de ser utilizado como primer operando limpia
///todo lo impreso en pantalla salvo el indicador de memoria ocupada.
/// Reestablece el estado original de las flags para iniciar un nuevo ciclo.
void aplicarM2() {
	if (flagOperando1 || flagResultadoEnPantalla)//condicion para escribir en el lugar del primer operando
			{
		lcdLimpiar(FILA_OPERANDOS, COLUMNA_PRIMER_OPERANDO, COLUMNA_FIN);//limpia la sección de los operandos y operador.
		lcdLimpiar(FILA_RESULTADO, COLUMNA_RESULTADO, COLUMNA_M);//limpia la sección del resultado, salvo la ultima celda que está imprimiendo M.

		//Imprime el valor guardado en memoria de M en el primer operando:
		if (numeroEnM1 < 0) {
			lcdImprimirNum(FILA_OPERANDOS, COLUMNA_PRIMER_OPERANDO,
					(long) numeroEnM1);	//Si es negativo, se imprime el numero desde la primer celda con su signo.
		} else {
			lcdImprimirNum(FILA_OPERANDOS, COLUMNA_PRIMER_OPERANDO + 1,
					(long) numeroEnM1);	//Si es positivo, se saltea la primer celda, ya que ésta está reservada para el signo.
		}
		numero1 = numeroEnM1;
		flagOperando1 = 0;
		flagOperador = 1;
		flagResultadoEnPantalla = 0;
		flagNuevaOperacion = 0;
	} else if (flagOperando2)//condicion para escribir en el lugar del segundo operando
	{
		lcdLimpiar(FILA_OPERANDOS, COLUMNA_SEGUNDO_OPERANDO, COLUMNA_FIN);//limpia la sección del segundo operando.

		//Imprime M en el segundo operando:
		if (numeroEnM1 < 0) {
			lcdImprimirNum(FILA_OPERANDOS, COLUMNA_SEGUNDO_OPERANDO,
					(long) numeroEnM1);	//Si es negativo, se imprime el numero desde la primer celda con su signo.
		} else {
			lcdImprimirNum(FILA_OPERANDOS, COLUMNA_SEGUNDO_OPERANDO + 1,
					(long) numeroEnM1);	//Si es positivo, se saltea la primer celda, ya que ésta está reservada para el signo.
		}
		numero2 = numeroEnM1;
		flagOperando2 = 0;
		flagResultado = 1;
	}
}
/// @briefPermite asignar valores al operando activo.
///
/// @param posicionOperando Establece la ubicacion del primer element dentro de la fila
/// @param posicionSigno Establece la ubicacion del signo dentro de la fila
/// @param NumOperando Establece el array del operando pertinente.
/// @param OpcOperando Establece el operando activo con sus respectivas flags
/// @param key el caracter a asignar en el array dentro de la posicion correspondiente a la iteracion.
void ingresarOperandos(int posicionOperando, int posicionSigno,char *NumOperando, int OpcOperando, char key) {
	if (i < 6 && (isdigit(key) || key == '_')) {
		if (OpcOperando == 1) {
			flagResultadoErroneo = 0;
		}		//DIFERENTE OP1

		if (key == '_' && hayNumero) {
			contadorSignoNegativo++;
		} else if (isdigit(key) && i < 5) {
			lcdImprimirCaracteres(0, posicionOperando + i, (String) key);		//1
			NumOperando[i] = key;
			i++;
			hayNumero = 1;
			if (OpcOperando == 1) {
				flagOperador = 1;
			}		//DIFERENCIAL OPERADOR1
			else {
				flagResultado = 1;
			}		//DIFERENCIAL OPERADOR2
		}

		if (contadorSignoNegativo % 2 == 1 && hayNumero) {
			lcdImprimirCaracteres(0, posicionSigno, "-");
			if (OpcOperando == 1) {
				numero1 = atof(NumOperando) * -1; /////Convierto string a double neg
			} else if (OpcOperando == 2) {
				numero2 = atof(NumOperando) * -1; /////Convierto string a double neg

			}
		} else if (contadorSignoNegativo % 2 == 0 && hayNumero) {
			lcdImprimirCaracteres(0, posicionSigno, " ");

			if (OpcOperando == 1) {
				numero1 = atof(NumOperando); /////Convierto string a double
			} else if (OpcOperando == 2) {
				numero2 = atof(NumOperando); /////Convierto string a double
			}
		}
	}
}
