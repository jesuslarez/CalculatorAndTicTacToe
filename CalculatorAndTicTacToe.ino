const int number[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};  //Numeros del 0 al 9 en hexadecimal
const int top = 34; //Constante con el numero del pin PORTC del Arduino correspondiente al pup
const int mid = 33; //Constante con el numero del pin PORTC del Arduino correspondiente al pcenter
const int bot = 31;//Constante con el numero del pin PORTC del Arduino correspondiente al pdown
const int left = 32; //Constante con el numero del pin PORTC del Arduino correspondiente al pup
const int right = 30; //Constante con el numero del pin PORTC del Arduino correspondiente al pcenter
const int pattern[] = {0x3F , 0x06, 0x5B, 0x4F , 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}; // Array correspondiente a los numeros del 0 al 9 para mostrar en los LEDs
const int leds[] = {254, 253, 251, 247};   //Leds utilizados
const int posJuego[] = {8, 10, 48, 50, 28, 30};   //Posiciones de la pantalla para el juego
const int posSpace[] = {7, 9, 11, 47, 49, 51};    //Posiciones de la pantalla para el juego
const int posicionesLlenas[3][3];                 //Matriz 3x3 para el juego

int units = 0;       //contendra el numero a mostrar para las unidades
int decimal = 0;     //contendra el numero a mostrar para las decenas
int cent = 0;        //contendra el numero a mostrar para las centenas
int mil = 0;         //contendra el numero a mostrar para los miles
long tiempo = 0;     //Variable para evitar rebote al pusar teclas
int segundo = 0;     //Variable para controlar el paso de 1 segundo
int first[] = { -1 , -1, -1, -1 , -1, -1, -1, -1, -1 , -1 };  //Primer operando de la calculadora
int second[] = { -1 , -1, -1, -1 , -1, -1, -1, -1, -1 , -1 }; //Segundo operando de la calculadora
int result = 0;     //Contendra Resultado final de la operacion
int cont = 0;       //Contador para Display 7 Segmentos y Teclado
int i = 0;          //Contador Auxiliar
int j = 0;          //Contador Auxiliar
int operador = 99;  //Indica el operador actual. (suma, resta, mult, div, ninguno)
int n = 0;          //Contendra el valor de la tecla presionada
int space = 0;      //Espacio del resultado para  el justificado
int funcion = 0;    //Indica la funcionalidad actual. 0 = Calculadora. 1 = Juego
int ganador = 0;    //Indica el ganador
int lleno = 0;      //Contador para saber si la matriz del juego se ha llenado

boolean ini = false;  //inicializador del juego
boolean gana = false;  //Indica si hay algun ganador
boolean modoJuego = false; //Indica si el modo juego esta activo
boolean modoCalculadora = true;  //Indica si el modo Calculadora esta activo
boolean iniciarCalc = false;     //Espera a iniciar a la calculadora
boolean buttonPushed = false;    //Indica si se ha pulsado una tecla
boolean showedResult = false;    //indica si se ha mostrado el resultado
boolean turno = true;            //Indica de quien es el turno actual en el juego.

void setup() {
  noInterrupts(); //Deshabilitar interrupciones
  DDRL = B00001111;
  PORTC = B11111000; //activamos el pull-up interno de las líneas entrada PC7-PC3
  DDRC = B00000001; //Definimos el PORTC de entrada salvo PC0 (salida)
  PORTA = B01111111;

  Serial.begin(9600);
  pinMode(42, INPUT);
  pinMode(43, INPUT);
  pinMode(44, INPUT);
  pinMode(45, INPUT);

  Serial.begin(9600);
  Serial3.begin(9600);

  Serial3.write(0xFE);
  Serial3.write(0x51);

  Serial3.write(0xFE);
  Serial3.write(0x46);

  Serial3.write(0xFE);
  Serial3.write(0x48);

  // modo normal de funcionamiento
  TCCR1A = 0;
  TCCR1B = 0;
  // cuenta inicial a cero
  TCNT1 = 0;
  // mode CTC
  TCCR1B |= (1 << WGM12);
  // prescaler N = 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // fintr = fclk/(N*(OCR1A+1)) --> OCR1A = [fclk/(N*fintr)] - 1
  // para fintr = 100Hz --> OCR1A = [16*10^6/(1024*100)] -1 = 155,25 --> 155
  OCR1A = 77; // para 200 Hz programar OCR1A = 77
  // enable timer1 compare interrupt (OCR1A)
  TIMSK1 |= (1 << OCIE1A);
  // habilitar interrupciones
  interrupts(); //Habilitar interrupciones
}

void loop() {

}

ISR(TIMER1_COMPA_vect) { //Timer que interrumpe cada 10 ms
  if (funcion == 0 ) {
    if (modoCalculadora) {
      calculadora();
      modoCalculadora = false;
    }
    ISR_calculadora();

  }
  if (funcion == 1) {
    if (modoJuego) {
      juego();
      modoJuego = false;
    }
    ISR_juego();
  }
}

void ISR_calculadora() {
  display1();

  segundo++;

  teclado1();

  cont++;
  if (cont == 4) {
    cont = 0;
  }


  if (iniciarCalc) {
    op();

    if (segundo % 200 == 0) {
      reloj();
    }
  } else {
    iniciarCalculadora();
  }

}
void teclado1() {
  //Funcion para el barrido para ver que botton se presiono
  switch (cont) {
    case 0:
      if (millis() > tiempo + 80) {
        tiempo = millis();
        if (digitalRead(42) == LOW) {
          Serial3.write(number[1]);
          n = 1;
          buttonPushed = true;
        } else if (digitalRead(43) == LOW) {
          Serial3.write(number[4]);
          n = 4;
          buttonPushed = true;
        } else if (digitalRead(44) == LOW) {
          Serial3.write(number[7]);
          n = 7;
          buttonPushed = true;

        } else if (digitalRead(45) == LOW) {
          Serial3.write(0xFE);
          Serial3.write(0x4E);
          backSpace();
        }
      }

      if (millis() > tiempo + 75 && digitalRead(44) == LOW && digitalRead(45) == LOW) {
        Serial3.write(0xFE);
        Serial3.write(0x51);
        clearScreen();
        funcion = 1;
        if (modoJuego == false) {
          modoCalculadora = false;
          modoJuego = true;
        }

      }

      break;
    case 1:
      if (millis() > tiempo + 80) {
        tiempo = millis();
        if (digitalRead(42) == LOW) {
          Serial3.write(number[2]);
          n = 2;
          buttonPushed = true;
        } else if (digitalRead(43) == LOW) {
          Serial3.write(number[5]);
          n = 5;
          buttonPushed = true;
        } else if (digitalRead(44) == LOW) {
          Serial3.write(number[8]);
          n = 8;
          buttonPushed = true;
        } else if (digitalRead(45) == LOW) {
          Serial3.write(number[0]);
          n = 0;
          buttonPushed = true;
        }
      }
      break;
    case 2:
      if (millis() > tiempo + 80) {
        tiempo = millis();
        if (digitalRead(42) == LOW) {
          Serial3.write(number[3]);
          n = 3;
          buttonPushed = true;
        } else if (digitalRead(43) == LOW) {
          Serial3.write(number[6]);
          n = 6;
          buttonPushed = true;
        } else if (digitalRead(44) == LOW) {
          Serial3.write(number[9]);
          n = 9;
          buttonPushed = true;
        } else if (digitalRead(45) == LOW) {
          Serial3.write(0xFE);
          Serial3.write(0x51);
          clearScreen();
        }
      }
      break;
    case 3:
      break;
  }
  if (buttonPushed == true && operador == 99) {
    first[i] = n;
    i++;
    buttonPushed = false;
  } else if (buttonPushed == true && operador != 99) {
    second[j] = n;
    j++;
    buttonPushed = false;
  }
}

void op() { //Metodo op que indica el operando actual
  if (operador == 99) {
    if (digitalRead(bot) == LOW) {
      if (millis() > tiempo + 80) {
        tiempo = millis();
        Serial3.write("/");
        operador = 3;
      }
    }
    if (digitalRead(top) == LOW)
    {
      if (millis() > tiempo + 80) {
        tiempo = millis();
        Serial3.write("*");
        operador = 2;
      }
    }
    if (digitalRead(left) == LOW)
    {
      if (millis() > tiempo + 80) {
        tiempo = millis();
        Serial3.write("-");
        operador = 0;
      }
    }
    if (digitalRead(right) == LOW)
    {
      if (millis() > tiempo + 80) {
        tiempo = millis();
        Serial3.write("+");
        operador = 1;
      }
    }
  }
  if (digitalRead(mid) == LOW)
  {
    if (millis() > tiempo + 80) {
      tiempo = millis();
      operacion();
      int aux = 1;
      aux = result;
      while (aux >= 1) {
        aux = aux / 10;
        space ++;
      }
      if (space == 0) {
        space = 1;
      }
      Serial3.write(0xFE);
      Serial3.write(0x45);
      Serial3.write(59 - space);
      Serial3.write("=");
      Serial3.print(result);
      space = 0;

    }
  }
}

int dameNum(int vec[]) { //Metodo que obtiene el numero del vector de numeros
  int res = 0;
  for (int q = 0; q < 10; q++) {
    if (vec[q] != -1) {
      res = res * 10 + vec[q];
    } else {
      break;
    }
  }
  return res;
}

void operacion() { //Metodo que indica la operacion a hacer
  switch (operador) {
    case 0: //Resta
      result = dameNum(first) - dameNum(second);
      break;
    case 1: //Suma
      result = dameNum(first) + dameNum(second);
      break;
    case 2: //Multiplicacion
      result = dameNum(first) * dameNum(second);
      break;
    case 3: //division
      result = dameNum(first) / dameNum(second);
      break;
  }
}

void reloj() { //Metodo que va actualizano el reloj
  units++;           // Aumenta el contador de las unidades
  if (units >= 10) { // si las unidades son mayores o iguales a 10, se ha salido del array
    units = 0;       //se reinicia el contador y se muestra un cero
    decimal++;     

    if (decimal == 6) { //si el valor de las decenas se sale del array,
      units = 0;        //Vuelve a colocar a 0 las unidades
      decimal = 0;
      cent ++;
      if (cent == 6) {
        units = 0;
        decimal = 0;
        cent = 0;
        mil++;
      } if (mil >= 10) {
        units = 0;
        decimal = 0;
        cent = 0;
        mil = 0;
      }
    }
  }
}

void calculadora() {
  Serial3.write("Modo Calculadora, presione enter");
  units = 0;
  decimal = 0;
  cent = 0;
  mil = 0;
  clearScreen();
}

void iniciarCalculadora() {
  if (digitalRead(mid) == LOW)
  {
    if (millis() > tiempo + 80) {
      clearScreen();
      tiempo = millis();
      iniciarCalc = true;
      Serial3.write(0xFE);
      Serial3.write(0x51);

    }
  }
}

void display1() {
  PORTL = 0;
  DDRA = 0;

  switch (cont) {
    case 0:
      DDRA = pattern[units];
      break;
    case 1:
      DDRA = pattern[decimal];
      break;
    case 2:
      DDRA = pattern[cent];
      break;
    case 3:
      DDRA = pattern[mil];
      break;
  }

  PORTL = leds[cont];
}

void clearScreen() { //Limpia las variables de calculadora
  for (int q = 0 ; q < 10 ; q++) {
    first[q] = -1;
    second[q] = -1;
  }
  operador = 99;
  result = 0;
  i = 0;
  j = 0;
}

void backSpace() { //Metodo para borrar un espacio
  if (operador != 99 && j > -1)
  {
    second[j] = -1;
    j--;
  }
  if (j == -1) {
    j++;
    operador = 99;
  }
  if (operador == 99 && i > -1 ) {
    first[i] = -1;
    i--;
  }
  if (i == -1) {
    i = 0;
  }
}

///////////////////////////////////JUEGO////////////////////////

void ISR_juego() {
  segundo++;

  display2();

  buttons();
  if (ini == true) {
        teclado2();
    if (gana == false) {

      checkRows();

      checkColums();

      checkDiagonal();

      if (segundo % 200 == 0) {
        tiempo1();
      }
    }
  }

  cont++;
  if (cont == 4) {
    cont = 0;
  }
}

void iniciarJuego() {//Coloca la matriz para el juego

  int i = 0;
  while (i < 6) {
    Serial3.write(0xFE);
    Serial3.write(0x45);
    Serial3.write(posJuego[i]);
    Serial3.write("|");
    i++;
  }
  i = 0;
  while (i < 6) {
    Serial3.write(0xFE);
    Serial3.write(0x45);
    Serial3.write(posSpace[i]);
    Serial3.write("_");
    i++;
  }

  for (int iF = 0; iF < 3; iF++) {
    for (int jC = 0 ; jC < 3; jC++) {
      posicionesLlenas[iF][jC] = 0;
    }
  }
}

void juego() {
  Serial3.write(" Modo Juego, presione up para iniciar");
  units = 0;
  decimal = 0;
  cent = 0;
  mil = 0;
  clearScreen();
}

void tiempo1() { //Metodo que va actualizano el reloj
  if (decimal == 0 && units == 0) { 
    timeUp();
    decimal = 0;
    units = 0;

    gana = true;
  } else {
    units--; 
    if (units < 0) { 
      units = 9; /
      decimal--; 
    }
  }
}

void teclado2() { //hace el barrido para detectar pulsaciones en el teclado
  switch (cont) {
    case 0:
      if (millis() > tiempo + 80) {
        tiempo = millis();
        if (digitalRead(42) == LOW) {
          decimal = 1;
          units = 0;
          Serial3.write(0xFE);
          Serial3.write(0x45);
          Serial3.write(7);
          if (turno) {

            Serial3.write("X");
            posicionesLlenas[0][0] = 1;
            turno = false;
          } else {
            Serial3.write("0");
            posicionesLlenas[0][0] = 2;
            turno = true;
          }
        } else if (digitalRead(43) == LOW) {
          decimal = 1;
          units = 0;
          Serial3.write(0xFE);
          Serial3.write(0x45);
          Serial3.write(47);
          if (turno) {

            Serial3.write("X");
            posicionesLlenas[1][0] = 1;
            turno = false;
          } else {
            Serial3.write("0");
            posicionesLlenas[1][0] = 2;
            turno = true;
          }

        } else if (digitalRead(44) == LOW) {
          decimal = 1;
          units = 0;
          Serial3.write(0xFE);
          Serial3.write(0x45);
          Serial3.write(27);
          if (turno) {

            Serial3.write("X");
            posicionesLlenas[2][0] = 1;
            turno = false;
          } else {
            Serial3.write("0");
            posicionesLlenas[2][0] = 2;
            turno = true;
          }

        }
      }
      if (millis() > tiempo + 75 && digitalRead(45) == LOW && digitalRead(42) == LOW) {
        Serial3.write(0xFE);
        Serial3.write(0x51);
        clearScreen();
        funcion = 0;
        if (modoJuego == true) {
          modoJuego = false;
        }
        modoCalculadora = true;
        iniciarCalc = false;
        
      }

      break;
    case 1:
      if (millis() > tiempo + 80) {
        tiempo = millis();
        if (digitalRead(42) == LOW) {
          decimal = 1;
          units = 0;
          Serial3.write(0xFE);
          Serial3.write(0x45);
          Serial3.write(9);
          if (turno) {

            Serial3.write("X");
            posicionesLlenas[0][1] = 1;
            turno = false;
          } else {
            Serial3.write("0");

            posicionesLlenas[0][1] = 2;
            turno = true;
          }
        } else if (digitalRead(43) == LOW) {
          decimal = 1;
          units = 0;
          Serial3.write(0xFE);
          Serial3.write(0x45);
          Serial3.write(49);
          if (turno) {

            Serial3.write("X");
            posicionesLlenas[1][1] = 1;
            turno = false;
          } else {
            Serial3.write("0");
            posicionesLlenas[1][1] = 2;
            turno = true;
          }
        } else if (digitalRead(44) == LOW) {
          decimal = 1;
          units = 0;

          Serial3.write(0xFE);
          Serial3.write(0x45);
          Serial3.write(29);
          if (turno) {

            Serial3.write("X");
            posicionesLlenas[2][1] = 1;
            turno = false;
          } else {
            Serial3.write("0");
            posicionesLlenas[2][1] = 2;
            turno = true;
          }
        } else if (digitalRead(45) == LOW) {

        }
      }
      break;
    case 2:
      if (millis() > tiempo + 80) {
        tiempo = millis();
        if (digitalRead(42) == LOW) {
          decimal = 1;
          units = 0;
          Serial3.write(0xFE);
          Serial3.write(0x45);
          Serial3.write(11);
          if (turno) {

            Serial3.write("X");
            posicionesLlenas[0][2] = 1;
            turno = false;
          } else {
            Serial3.write("0");
            posicionesLlenas[0][2] = 2;
            turno = true;
          }
        } else if (digitalRead(43) == LOW) {
          decimal = 1;
          units = 0;
          Serial3.write(0xFE);
          Serial3.write(0x45);
          Serial3.write(51);
          if (turno) {

            Serial3.write("X");
            posicionesLlenas[1][2] = 1;
            turno = false;
          } else {
            posicionesLlenas[1][2] = 2;
            Serial3.write("0");
            turno = true;
          }
        } else if (digitalRead(44) == LOW) {
          decimal = 1;
          units = 0;
          Serial3.write(0xFE);
          Serial3.write(0x45);
          Serial3.write(31);
          if (turno) {

            Serial3.write("X");
            posicionesLlenas[2][2] = 1;
            turno = false;
          } else {
            Serial3.write("0");
            posicionesLlenas[2][2] = 2;
            turno = true;
          }
        } else if (digitalRead(45) == LOW) {

        }
      }
      break;
    case 3:
      break;
  }
}

void checkRows () { //Comprueba si alguien gana, filas


  int fila = 0;
  ///Horizontales
  while (fila < 3 ) {
    if (posicionesLlenas[fila][0] != 0)
    {
      if (posicionesLlenas[fila][1] != 0 && posicionesLlenas[fila][1] == posicionesLlenas[fila][0])
      {
        if (posicionesLlenas[fila][2] != 0 && posicionesLlenas[fila][2] == posicionesLlenas[fila][1])
        {

          if (posicionesLlenas[fila][2] == 1) {
            ganador = 49;
          } else if (posicionesLlenas[fila][2] == 2) {
            ganador = 50;
          }
          gana = true;
          endGame();
          break;
        }
      }
    }
    fila++;
  }
  lleno = 0;
  for (int iF = 0; iF < 3; iF++) {
    for (int jC = 0 ; jC < 3; jC++) {
      if (posicionesLlenas[iF][jC] != 0) {
        lleno++;
      }

    }
  }
  if (lleno == 9) {
    gana = true;
    endGame();

  }
}

void checkColums() {//Comprueba si alguien gana, columnas
  int colum = 0;
  while (colum < 3) {
    if (posicionesLlenas[0][colum] != 0)
    {
      if (posicionesLlenas[1][colum] != 0 && posicionesLlenas[1][colum] == posicionesLlenas[0][colum])
      {
        if (posicionesLlenas[2][colum] != 0 && posicionesLlenas[2][colum] == posicionesLlenas[1][colum])
        {

          if (posicionesLlenas[2][colum] == 1) {
            ganador = 49;
          } else if (posicionesLlenas[2][colum] == 2) {
            ganador = 50;
          }
          gana = true;
          endGame();
          break;
        }
      }
    }
    colum++;
  }
}

void checkDiagonal() {//Comprueba si alguien gana, diagonales 
  if (posicionesLlenas[0][0] != 0)
  {
    if (posicionesLlenas[1][1] != 0 && posicionesLlenas[1][1] == posicionesLlenas[0][0])
    {
      if (posicionesLlenas[2][2] != 0 && posicionesLlenas[2][2] == posicionesLlenas[1][1])
      {

        if (posicionesLlenas[2][2] == 1) {
          ganador = 49;
        } else if (posicionesLlenas[2][2] == 2) {
          ganador = 50;
        }
        gana = true;
        endGame();
      }
    }

  }
  if (posicionesLlenas[0][2] != 0)
  {
    if (posicionesLlenas[1][1] != 0 && posicionesLlenas[1][1] == posicionesLlenas[0][2])
    {
      if (posicionesLlenas[2][0] != 0 && posicionesLlenas[2][0] == posicionesLlenas[1][1])
      {

        if (posicionesLlenas[2][0] == 1) {
          ganador = 49;
        } else if (posicionesLlenas[2][0] == 2) {
          ganador = 50;
        }
        gana = true;
        endGame();
      }
    }

  }

}

void buttons() {
  if (digitalRead(top) == LOW)
  {
    if (millis() > tiempo + 80) {
      Serial3.write(0xFE);
      Serial3.write(0x51);

      iniciarJuego();
      ini = true;
      decimal = 1;
      units = 0;

    }
  }
  if (digitalRead(mid) == LOW)
  {
    if (millis() > tiempo + 80) {
      Serial3.write(0xFE);
      Serial3.write(0x51);
      gana = false;
      decimal = 1;
      units = 1;
      iniciarJuego();
    }
  }
}

void display2() {
  PORTL = 0;
  DDRA = 0;

  switch (cont) {
    case 0:
      DDRA = pattern[units];
      break;
    case 1:
      DDRA = pattern[decimal];
      break;
    case 2:
      DDRA = pattern[cent];
      break;
    case 3:
      DDRA = pattern[mil];
      break;
  }

  PORTL = leds[cont];
}

void endGame() {
  Serial3.write(0xFE);
  Serial3.write(0x51);
  if (lleno == 9) {
    Serial3.write(" Empate! Reiniciar? ");
    lleno = 0;
  } else {
    Serial3.write(" el  jugador ");
    Serial3.write(ganador);
    Serial3.write(" Ha ganado!");
  }
}

void timeUp() {
  Serial3.write(0xFE);
  Serial3.write(0x51);

  if (turno) {
    ganador = 49 ;
  } else {
    ganador = 50;
  }
  Serial3.write(" Tiempo agotado, el  jugador ");
  Serial3.write(ganador);
  Serial3.write(" Ha ganado");
}
