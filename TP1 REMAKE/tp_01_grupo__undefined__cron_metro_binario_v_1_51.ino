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

//--- DEFINES ---//
#define ZERO_TO_HERO 0 // Inicio y Seteo del Contador a cero
#define BUTTON_START 2 // Pin del pulsador de Inicio
#define BUTTON_RESET 3 // Pin del pulsador de Reseteo
#define FIRST_LED 4 // Primer Pin de los Leds
#define LAST_LED 13 // Ultimo Pin de los Leds
#define BASE_MILLIS_SECONDS 1000 // Segundos expresados en milisegundos
#define MAX_SECONDS 1023 //! Maxima cantidad de segundos a contar

// Definido de esta forma para que al modificar la posicion del primer y/o ultimo led se adecue a la cantidad leds
#define TOTAL_LED (LAST_LED - FIRST_LED + 1)

//--- FIN DEFINES--//

// delays de las tareas en ms
const int DELAY_BOTONES = 100;
const int DELAY_LEDS = 100;
const int DELAY_CONTADOR = 1000;

int contador = ZERO_TO_HERO;

const int leds_arr[TOTAL_LED] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

unsigned long ultimo_tiempo_botones;
unsigned long ultimo_tiempo_leds;
unsigned long ultimo_tiempo_contador;

int pausa = 1;


void setup() {
  Serial.begin(9600);
  inicializarPines();
}

/* Inicializa pausado (en 1)
No ejecuta tareas que cuentan y prenden LEDS,
solo checkea el estado de los botones.*/
void loop() {


  // Tarea 1
  /* Establece la frecuencia con la que se puede ejecutar la tarea de
   evaluar el estado de los botones*/
  if (millis() - ultimo_tiempo_botones >= DELAY_BOTONES) {
    tarea_check_botones();

    ultimo_tiempo_botones = millis();
  }

  if (!pausa) {//al no estar pausado se habilitan las tareas de prender leds y aumentar contador, ambos funcionan de forma independiente

      // Tarea 2
      /* Establece la frecuencia con la que se puede ejecutar la tarea de
       operar los leds */
    if (millis() - ultimo_tiempo_leds >= DELAY_LEDS) {
      tarea_leds(contador, TOTAL_LED); //los leds pueden tomar cualquier numero, solo muestran el numero actual del contador

      ultimo_tiempo_leds = millis();
    }

      // Tarea 3
      /* Establece la frecuencia con la que se puede ejecutar la tarea de
       aumentar el contador de segundos y lo imprime en serial(consola/monitor).
       Al alcanzar el maximo de segundos (1023)
       reinicia el contador y los leds */
    if (millis() - ultimo_tiempo_contador >= DELAY_CONTADOR) {
      if (contador >= MAX_SECONDS) {
        pausa = 1;
        contador = 0;
        tarea_leds(contador, TOTAL_LED);
        return;
      }

      contador++;

      Serial.print("Seconds: ");
      Serial.print(contador);
      Serial.print(" | Binary: ");

      int contador_bin[TOTAL_LED];

      dec_a_bin(contador, contador_bin, TOTAL_LED );

	//Para mostrar los segundos en numeros binarios los convierte en un array
	//Imprime los segundos en binario
      for (int i = TOTAL_LED -1 ; i >= 0; i--) {
        Serial.print(contador_bin[i]);
      }

      Serial.println();

      ultimo_tiempo_contador = millis();
    }
  }
  delay(5);
}


// Inicializa los pines,
/*Le asigna un valor de salida a cada pin recorriendo el array.
Además, asigna valor de entrada a los pines de los pulsadores.
*/
void inicializarPines(){
	for (int i = 0; i < TOTAL_LED; i++) {
		pinMode(leds_arr[i], OUTPUT);
	}

  pinMode(BUTTON_START, INPUT);
  pinMode(BUTTON_RESET, INPUT);
}

//Checkea el estado de los botones
/*Se usa variables estaticas para recordar el último estado de los pines
entre llamadas de función.
Se implementa el Antirrebote, evaluando la persistencia del estado.*/
void tarea_check_botones() {

  int btn_start_ahora = digitalRead(BUTTON_START);
  static int btn_start_antes = 0;
  int btn_reset_ahora = digitalRead(BUTTON_RESET);
  static int btn_reset_antes = 0;

  if(btn_start_ahora && !btn_start_antes) // ANTIRREBOTE
  {
	pausa = !pausa;
    digitalWrite(BUTTON_START,pausa);
   }
  btn_start_antes= btn_start_ahora;

  if (btn_reset_ahora && !btn_reset_antes) //ANTIRREBOTE
  {
	digitalWrite(BUTTON_RESET,1);
    contador = 0;
    tarea_leds(contador, TOTAL_LED);
    Serial.print("\n\n\n---------Reinicio del cronometro---------\n\n\n");
  }
  btn_reset_antes= btn_reset_ahora;
}

//Controla los leds:
/*Ingresa el número en decimal del contador de segundos,
lo convierte a binario llamando a otra función,
y lo muestra en los leds*/
void tarea_leds(int num, const int digits) {
  int num_bin[digits];

  dec_a_bin(num, num_bin, digits);

  for (int i = 0; i < digits; i++) {
    digitalWrite(leds_arr[i], num_bin[i]);
  }
}

//Convierte un decimal a binario representado como un array,
//asigna digito a digito a cada led.
void dec_a_bin(int dec, int* bin_arr, const int size) {
  for (int i = 0; i < size; i++) {
    if (dec % 2) {
      bin_arr[i] = 1;
    } else {
      bin_arr[i] = 0;
    }
    dec = dec / 2;
  }
}

