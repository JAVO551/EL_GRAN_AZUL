
///////////////////////////////////////VARIABLES PARA INFRARROJOS////////////////////////////////////////////////
int c =0;
int c2 =0;
int INFRA_PIN [8] = {26,25,33,32,35,34,39,36}; //PIN 36 ES EL PRIMER INFRAROJO QUE ES EL DE HASTA LA DERECHA
int INFRA_VNEGROS [8];
int INFRA_VBLANCOS [8];
int INFRA_UMBRAL [8];
int INFRA_VALORES [8];
int INFRA_DIGITALES [8];
int BOTON_CALIBRACION = 27;

//Para la posición
int SUMA_PESOS;
int SUMA_DIGITOS;
int POSICION;
int POS_ANTERIOR;

///////////////////////////////////////VARIABLES PARA MOTORES////////////////////////////////////////////////

int PWMD_PIN = 16;
int PWMI_PIN =17;
int DERECHA_PIN_1= 18;
int DERECHA_PIN_2= 19;
int IZQUIERDA_PIN_1 = 22;
int IZQUIERDA_PIN_2 = 23;

int VEL_ESTANDAR = 240; //3500
int VEL_MAX = 255; //3600
int VEL_MIN = 255; //2500


/////////////////////////////////PARA EL PID///////////////////////////////////////

float KP = 0.02;
float KD = 2;
float KI = 0;
//0.3,7,0.002
//0.2, 11, 0.0018
//0.001 + 3 +0= velocidad decente y pudo con esquinas decente, como si el 3 tuviera más peso, pero ligeramente más rápido. Se nota el tambaleo.
//0.02 + 2 +0= velocidad decente y pudo con esquinas, como si el 2 tuviera más peso, pero ligeramente más rápido. Se nota un poco el tambaleo. MI FAVORITA HASTA AHPRA

/* NÚMERO, EL PUN
INDIVIDUAL: VELOCIDAD. DESEMPEÑO EN PISTA CIRCULAR. DESEMPEÑO CONTRA ESQUINAS

-Proporcional = LA COMPARACION ES CONTRA 0.3 = POCO TAMBALEO. PERO POR SI SOLO NO PUEDE CON ESQUINAS

0.1 aMantuvo la velocidad y disminuyo el tambaleo. No pudo con esquinas
No note muchas diferencias entre 0.001 y 0.1 POCO TAMBALEO. . Se salio de la pista circular. No pudo contra esquinas.
No note muchas diferencias entre 0.00001 y 0.001 POCO TAMBALEO. . Se salio de la pista circular. No pudo contra esquinas

0.7 Mantuvo velocidad en general, pero en algunas curvas se atoro unos milisegundos.. No pudo contra esquinas, parecio responde un poco.
3 Bajo un poquito de velocidad (2s aprox) y empezo a tambalear un poco. Pudo algunas veces contra una esquina, creo que por el tambaleo que da más tiempo de respuesta
10 tambaleo bastante más. Pudo con las esquinas de manera ok
50 tambaleo ligeramente más 4s más que 0.3. Pudo más decentemente con esquias
50 no hubo tanata diferencia respecto a 10
10 000 no note mucho cambio respecto a 50
1 000 000 no note mucho cambio respecto a 50. No vi exactamentee como sucedio, pero se descarrilo en un punto

Con todos los valores se consiguio completar la pista

CONCLUSIÓN: Para este robot, un valor de 0.3 da una buena velocidad, pero tiene problemas con cambios abruptos como esquinas.
Una valor más equilibrado fue de 3, que disminuyo un poco la velocidad y empezo a poder con las esquinas.
Valores más altos disminuye bastantela velocidad, pero da más control.
Valores más bajos aumenta la velocidad, pero hasta puede descarrilarse.
float KP = 3;
float KD = 0;
float KI = 0;

-INTEGRA = LA COMPARACION ES CONTRA 7 = tIENE UN COMPORTAMIENTO MUY SIMILAR AL 10 DEL PROPORCIONAL... pero se atoro en algunas esquinas

Despues de varias pruebas con valores de 0.3, 0.1, 3, 7 y 10 pude notar que el comportamiento era muy parecido si es que no igual
a la parte Proporcional. Siendo 3 un buen equilibrio entre todo. Solo que senti que 3 era un poco más responsivo para las esquinas en algunas situaciones.
Tengo la hipotesis que usar un valor muy bajo de Kp para correcciones pequeñas y ganar buena velocidad necesitar un valor ki 10 veces más grande.
Para no sacrificar tanta velocidad y aprovechar l ligera mayor responsividad de esto, tal vez sea más responsiva por que los errores acumulados se renuevan:
En una linea recta no habra tantos errores malos antiguos dando más velocidad y en curva los errores se actualizaran a unos grandes rapidamente.

COCLUSIÓN = Muy similar al Kp por si solo. Un buen equilibrio en 3.
Hipótesis: Más responsivo por lo que más grande que Kp

*/
//Para la función integradora
int error1 = 0;
int error2 = 0;
int error3 = 0;
int error4 = 0;
int error5 = 0;
int error6 = 0;

int ERROR = 0;
int E_ANTERIOR = 0;
int integral = 0;
int derivativa = 0;
int DIFERENCIAL_MOV = 0;
int PUNTO_PIVOTE = 350;

void lecturas_infrarojo ();
void motores(int vel_izq, int vel_der);
void GIROS ();
void PID ();

///////////////////////////////////////SETUP////////////////////////////////////////////////

void setup() {

///PARA LOS MOTORES
ledcAttach (PWMD_PIN,150000,8);
ledcAttach (PWMI_PIN,150000,8);
pinMode (DERECHA_PIN_1,OUTPUT);
pinMode (DERECHA_PIN_2,OUTPUT);
pinMode (IZQUIERDA_PIN_1,OUTPUT);
pinMode (IZQUIERDA_PIN_2,OUTPUT);
//15 000 = 8.84

///////////////INICIO DE TODO CON LA CALIBRACION///////////////////////
Serial.begin (115200);
delay (3000);
pinMode (BOTON_CALIBRACION, INPUT);

analogReadResolution (12); 
delay (100);
Serial.println ("Esperando señal para calibrar BLANCOS");
while (!digitalRead (BOTON_CALIBRACION)) {
}
Serial.println ("Comenzamos Calibracion, Primero Blancos");
calibracion_blancos ();
Serial.println ("Esperando señal para calibrar NEGROS");
delay (3000);
while (!digitalRead (BOTON_CALIBRACION)) {
}
Serial.println ("Comenzamos Calibracion, Primero Negros");
calibracion_negros ();
Serial.println ("Vamos con la lectura de datos");
while (!digitalRead (BOTON_CALIBRACION)) {
}
}



void loop() {

GIROS ();
lecturas_infrarojo ();
PID ();
 
}






///////////////////////////////////////FUNCIONES PARA INFRARROJOS////////////////////////////////////////////////


void calibracion_blancos () {

  //Detectando los valores de los blancos y negros en esta luz
  INFRA_VBLANCOS [0] =0;
  
  for (c=0; c<8;c++) {
    for (c2=0;c2<50;c2++){
    INFRA_VBLANCOS [c] += analogRead (INFRA_PIN [c]);
    Serial.println (INFRA_VBLANCOS [c]);
    }
    INFRA_VBLANCOS [c] = (INFRA_VBLANCOS [c])/50;
 }
 
}

void calibracion_negros () {
  INFRA_VNEGROS [0] =0;
  for (c=0; c<8;c++) {
    for (c2=0;c2<50;c2++){
    INFRA_VNEGROS [c] += analogRead (INFRA_PIN [c]);
    Serial.println (INFRA_VNEGROS [c]);
    }
    INFRA_VNEGROS [c] = (INFRA_VNEGROS [c])/50;
    
    INFRA_UMBRAL [c] = (INFRA_VNEGROS [c] + INFRA_VBLANCOS [c])/2; //Asignando el umbral a de luz a cada sensor
    Serial. print ("UMBRAL: ");
    Serial.println (INFRA_UMBRAL [c]);
 }
}

void lecturas_infrarojo () {
  //Lecturas que nos daran la posición para el PID
  for (c=0;c<8;c++) {
  INFRA_VALORES [c] = analogRead (INFRA_PIN [c]);
  if (INFRA_VALORES [c] <= INFRA_UMBRAL [c]) {
    INFRA_DIGITALES [c] = 0; //0 PARA LOS BLANCOS
  } else {
    INFRA_DIGITALES [c] = 1; //1 PARA LOS NEGROS
  }
  }

  //Obteniendo posición mediante promedio ponderado

 SUMA_PESOS = (INFRA_DIGITALES [0] * 700)+(INFRA_DIGITALES [1] * 600)+(INFRA_DIGITALES [2] * 500)+(INFRA_DIGITALES [3] * 400)+(INFRA_DIGITALES [4] * 300)+
 (INFRA_DIGITALES [5] * 200)+(INFRA_DIGITALES [6] * 100);

 SUMA_DIGITOS = INFRA_DIGITALES [0] + INFRA_DIGITALES [1] + INFRA_DIGITALES [2] + INFRA_DIGITALES [3] + INFRA_DIGITALES [4] + INFRA_DIGITALES [5] + 
 INFRA_DIGITALES [6] + INFRA_DIGITALES [7];

if (SUMA_DIGITOS != 0) {
 POSICION = SUMA_PESOS/SUMA_DIGITOS;
} else if (POS_ANTERIOR <= 100) {
  POSICION = 0;
} else if (POS_ANTERIOR >= 600) {
  POSICION = 700;
}
POS_ANTERIOR = POSICION;

  for (c=0; c<8;c++) {
  Serial.print (INFRA_DIGITALES [c]);
  
  Serial.print ("\t");
  }
  Serial.println (POSICION); //EL DATO SHIDO
  //delay (10); //OJO

}


//////////////////////////////////////////////FUNCIONES PARA MOTORES////////////////////////////////////////////////

void motores (int vel_izq, int vel_der) {
if (vel_der < 0) {
  digitalWrite (DERECHA_PIN_1,HIGH);
  digitalWrite (DERECHA_PIN_2,LOW);
  ledcWrite (PWMD_PIN,vel_der*(-1));
} else {
  digitalWrite (DERECHA_PIN_1,LOW);
  digitalWrite (DERECHA_PIN_2,HIGH);
  ledcWrite (PWMD_PIN,vel_der);
}
if (vel_izq < 0) {
  digitalWrite (IZQUIERDA_PIN_1,HIGH);
  digitalWrite (IZQUIERDA_PIN_2,LOW);
  ledcWrite (PWMI_PIN,vel_izq*(-1));
} else {
  digitalWrite (IZQUIERDA_PIN_1,LOW);
  digitalWrite (IZQUIERDA_PIN_2,HIGH);
  ledcWrite (PWMI_PIN,vel_izq);
}
}

//////////////////////////////////////////////FUNCIONES PARA PID////////////////////////////////////////////////

void PID () {

  ERROR = POSICION - PUNTO_PIVOTE; //P "error = proporcional"
  integral = error1+error2+error3+error4+error5+error6; //I
  error6 = error5;
  error5 = error4;
  error4 = error3;
  error3 = error2;
  error2 = error1;
  error1 = ERROR;
  derivativa = ERROR - E_ANTERIOR; //D

  DIFERENCIAL_MOV = (KP*ERROR) + (KI*integral) + (KD*derivativa);

  if (DIFERENCIAL_MOV > VEL_ESTANDAR) {
    DIFERENCIAL_MOV = VEL_ESTANDAR;
  } else if (DIFERENCIAL_MOV < -VEL_ESTANDAR) {
    DIFERENCIAL_MOV = -VEL_ESTANDAR;
  }

  //CONTROL DE MOTORES
  if (DIFERENCIAL_MOV < 0) {
    motores (VEL_ESTANDAR,VEL_ESTANDAR+ DIFERENCIAL_MOV);
  } else {
    motores (VEL_ESTANDAR - DIFERENCIAL_MOV, VEL_ESTANDAR);
  }
}

void GIROS () {
  if ( POSICION <= 100) {
    motores(VEL_MAX,-VEL_MIN);
  }
  if (POSICION >= 600) {
    motores(-VEL_MIN,VEL_MAX);
  }
}





