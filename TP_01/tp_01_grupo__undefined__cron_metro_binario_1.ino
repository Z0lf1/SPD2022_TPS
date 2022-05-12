/*
 *	Trabajo Práctico 1: Cronómetro Binario
 *
 *	Al iniciar la simulacion, todos los led deben comenzar apagados.
 *	Cada 1 segundo que transcurra, se deben encender los led
 *	de acuerdo al numero que representan.
 *
 *	Ejemplo:
 *
 *	El numero 15 (0000001111) se representaria encendiendo los
 *	leds AMARILLO y NARANJA.
 *	Al pasar al numero 16 (0000010000), se deben apagar todos
 *	los led anteriores y encenderse uno de los leds VERDES.
 *	Notese, en el ejemplo los 0 representan los led apagados
 *	y los 1 los led encendidos).
 *
 *	-------------------------------------------------------
 *
 *	Al presionarse el boton START, debe iniciar el cronometro.
 *	Volver a presionarlo hace que la secuencia se detenga.
 *  (Como si se hubiese presionado pausa).
 *	Al presionarse el boton RESET, el cronometro
 *	debe reiniciarse y arrancar de 0.
 *
 *	Tip: Modularizar la función que controla el encendido de los
 *	LEDS y de ser posible, todo el código para evitar repetir lineas lo mas
 *posible. Usar millis para controlar el tiempo del contador para que el cambio
 *de los leds encendidos sea perceptible para el ojo humano y documentar cada
 *función creada en el código. Un breve comentario que diga que es lo que hace
 *esa función y de corresponder, que retorna).
 */



//##### Una Ayudita #####
//--- Defines ---//
#define ZERO_TO_HERO 0  //? To start the counter from 0
#define BUTTON_START 2
#define BUTTON_RESET 3
#define FIRST_LED 4              //? First pin of the leds
#define LAST_LED 13              //? Last pin of the leds
#define BASE_MILLI_SECONDS 1000  //? Secods expresed in miliseconds
#define MAX_SECONDS 1023         //! Max amount of secods to show
//--- End Defines ---//

#define TOTAL_LED (LAST_LED - FIRST_LED + 1)

// delays de las tareas en ms
const int DELAY_POLLING = 100;
const int DELAY_LEDS = 100;
const int DELAY_CONTADOR = 1000;

int contador = 0;

const int leds_arr[TOTAL_LED] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

void tarea_check_botones();
void tarea_leds(int num);
void dec_a_bin(const int dec, int* bin, const int digits);

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < TOTAL_LED; i++) {
    pinMode(leds_arr[i], OUTPUT);
    digitalWrite(leds_arr[i], LOW);
  }

  pinMode(BUTTON_START, INPUT);
  pinMode(BUTTON_RESET, INPUT);
}

unsigned long ultimo_tiempo_botones;
unsigned long ultimo_tiempo_leds;
unsigned long ultimo_tiempo_contador;

int pausa = 1;

void loop() {//inicializa pausado (en 1) No ejecuta tareas que cuentan y prenden LEDS, solo checkea el estado de los botones
  // tarea 1 - poll inputs
  if (millis() - ultimo_tiempo_botones >= DELAY_POLLING) {
    tarea_check_botones();

    ultimo_tiempo_botones = millis();
  }

  if (pausa == 0) {//al no estar pausado se habilitan las tareas de prender leds y aumentar contador, ambos funcionan de forma independiente
    // tarea 2 - prender leds //por un lado encienden los leds
    if (millis() - ultimo_tiempo_leds >= DELAY_LEDS) {
      tarea_leds(contador); //los leds pueden tomar cualquier numero, solo muestran el numero actual del contador

      ultimo_tiempo_leds = millis();
    }

    // tarea 3 - aumentar contador // y por otro aumenta el contador
    if (millis() - ultimo_tiempo_contador >= DELAY_CONTADOR) {
      if (contador >= MAX_SECONDS) {
        pausa = 1;
        return;
        }
	  
      contador++;

      Serial.print("Seconds: ");
      Serial.print(contador);
      Serial.print(" | Binary: ");

      int contador_bin[TOTAL_LED];	

      dec_a_bin(contador, contador_bin, TOTAL_LED );
	//Para mostrar los segundos en numeros binarios los convierte en un array 
      for (int i = TOTAL_LED -1 ; i >= 0; i--) {
        Serial.print(contador_bin[i]);
      }

      Serial.println();

      ultimo_tiempo_contador = millis();
    }
  }
  delay(50);
}

// aca me fijo que botones estan presionados
void tarea_check_botones() {
  int btn_start = digitalRead(BUTTON_START);
  int btn_reset = digitalRead(BUTTON_RESET);

  if (btn_start) {
    pausa = !pausa;
  }
  if (btn_reset) {
    contador = 0;
  }
}

//control de los leds:Ingresa numero y lo convierte a binario representado en un array 
void tarea_leds(int num) {
  const int digits = 10;
  int num_bin[digits];

  dec_a_bin(num, num_bin, digits);

  for (int i = 0; i < TOTAL_LED; i++) {
    digitalWrite(leds_arr[i], num_bin[i]);
  }
}
//Convierte un decimal a binario representado como un array, asigna digito a digito a cada led. 
void dec_a_bin(int dec, int* bin, const int digits) {
  for (int i = 0; i < digits; i++) {
    if (dec % 2) {
      bin[i] = 1;
    } else {
      bin[i] = 0;
    }
    dec = dec / 2;
  }
}