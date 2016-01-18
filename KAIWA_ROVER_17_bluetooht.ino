//-----------------------------------------------------------------
//          Programa hecho por: Konrad Peschka
//          Entidad: Kaiwa Technology
//          Fecha: Noviembre 2015
//-----------------------------------------------------------------


//--------------defino librerias y variables para Ultrasonidos------------------


#include <AFMotor.h>

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

#include <NewPing.h>

#define MAX_DISTANCE 100 // Maximum distance (in cm) to ping.

NewPing sonar_der(A0,A1, MAX_DISTANCE);
NewPing sonar_centro(A2,A3, MAX_DISTANCE);
NewPing sonar_izq(A4,A5, MAX_DISTANCE);

#include <SoftwareSerial.h>
SoftwareSerial BTserial(9,10); // RX | TX

//--------------------------------------
//     Definicion Variables del Programa
//--------------------------------------

//#define debug                  //comentando o descomentando esta linea activo el codigo para debug


const int distancia_critica = 20;
const int distancia_deteccion=35;

const int tiempo_stop=20;
const int tiempo_giro=50;
const int tiempo_atras=100;
const int tiempo_frente=20;
const int tiempo_bluetooht=20;
const int tiempo_lectura_bt=2000;

int distancia_der=0;
int distancia_der_anterior=30;
int distancia_centro=0;
int distancia_centro_anterior=30;
int distancia_izq=0;
int distancia_izq_anterior=30;

const int cantidad=3;
int matriz_der[cantidad];
int matriz_centro[cantidad];
int matriz_izq[cantidad];


int velocidad=250;
const int velocidad_inicial=150;
const int velocidad_incremento=5;
int velocidad_frente=velocidad_inicial;
int velocidad_atras=velocidad_inicial;

int aux_matriz=0;
unsigned long aux_millis=0;

int modo_bluetooht=0;       //cero indica que el bluetooht no esta conectado y modo automatico

//----------------------------------------
//  Funcion SETUP
//----------------------------------------

void setup()
{
  
#ifdef debug
 Serial.begin(9600);
#endif

  pinMode(MOTORLATCH, OUTPUT);
  pinMode(MOTORENABLE, OUTPUT);
  pinMode(MOTORDATA, OUTPUT);
  pinMode(MOTORCLK, OUTPUT);

  pinMode(10, INPUT);
 
 movimiento_stop();

 for(int i=0;i<cantidad;i++)
 {
  matriz_der[i]=sonar_der.ping_cm();
  matriz_centro[i]=sonar_centro.ping_cm();
  matriz_izq[i]=sonar_izq.ping_cm();
 }

 BTserial.begin(9600);           //puerto abierto para el bluetooht

 delay(5000);    //cinco segundos de tiempo para que arranque el robot ya que no tengo boton de inicio.

 millis();
}

//----------------------------------------
//  Funcion principal del codigo
//----------------------------------------

void loop()
{

while(1)
{
int variable_bluetooht=lectura_bluetooht();

if(variable_bluetooht==55)
modo_bluetooht=0;    //si es 55, BT desconectado

else
modo_bluetooht=1;    //va a modo bluetooht

    
//-------------------Modo Automatico------------------------

while(modo_bluetooht==0)   //modo automatico, del bluetooht no llega nada
{

  if((millis()-aux_millis)>tiempo_lectura_bt)    //me sirve para leer BT cada 2 segundos
  {
    aux_millis=millis();  //recargo el auxiliar
    variable_bluetooht=lectura_bluetooht();
    if(variable_bluetooht==55)
    modo_bluetooht=0;         
    else
    modo_bluetooht=1;     //cuando BT se conecta, salgo del while
  }
  
switch (funcion_ultrasonido()) //leo ultrasonido
{

    case 0:  // Hay algo critico a la derecha
     movimiento_stop();
     delay(tiempo_stop);
     movimiento_atras();
     delay(tiempo_atras);
     movimiento_izquierda();
     delay(tiempo_giro);

    #ifdef debug
    Serial.print("Caso 0");
    #endif

    break;
   
    case 1:  // Hay algo critico en el centro
    movimiento_stop();
    delay(tiempo_stop);
    movimiento_atras();
    delay(tiempo_atras);
    movimiento_izquierda();
    delay(tiempo_giro);

    #ifdef debug
    Serial.print("Caso 1");
    #endif

    break;
      
    case 2:  // Hay algo critico a la izquierda
    movimiento_stop();
    delay(tiempo_stop);
    movimiento_atras();
    delay(tiempo_atras);
    movimiento_derecha();
    delay(tiempo_giro);

    #ifdef debug
    Serial.print("Caso 2");
    #endif

     break;
      
    case 3:  //todo libre adelante
    movimiento_stop();
    delay(tiempo_stop);
    movimiento_frente();
    delay(tiempo_frente);

    #ifdef debug
    Serial.print("Caso 3");
    #endif

      break;
      
    case 4:  //hay algo a la izquierda
    movimiento_stop();
    delay(tiempo_stop);
    movimiento_derecha();
    delay(tiempo_giro);

    #ifdef debug
    Serial.print("Caso 4");
    #endif

      break;
      
    case 5: // hay algo en el medio
    movimiento_stop();
    delay(tiempo_stop);
    movimiento_izquierda();
    delay(tiempo_giro);

    #ifdef debug
    Serial.print("Caso 5");
    #endif

      break;
      
    case 6:  //hay algo a la izquierda
    movimiento_stop();
    delay(tiempo_stop);
    movimiento_derecha();
    delay(tiempo_giro);

    #ifdef debug
    Serial.print("Caso 6");
    #endif

      break;
      
    case 7:  //hay algo derecha
     movimiento_stop();
    delay(tiempo_stop);
    movimiento_izquierda();
    delay(tiempo_giro); 

    #ifdef debug
    Serial.print("Caso 7");
    #endif
   
    break;
      
    case 8:  //no hacemos nada

    #ifdef debug
    Serial.print("Caso 8");
    #endif

      break;
      
    case 9:  //hay algo a la derecha
    movimiento_stop();
    delay(tiempo_stop);
    movimiento_izquierda();
    delay(tiempo_giro); 

    #ifdef debug
    Serial.print("Caso 9");
    #endif

      break;
      
    case 10:  //no hacemso nada

    #ifdef debug
    Serial.print("Caso 10");
    #endif

      break;

     case 11:  //no hacemso nada

    #ifdef debug
    Serial.print("Caso 11");
    #endif

      break;
      
  }
  }


//-------------------Modo Manual Bluetooht------------------------

while(modo_bluetooht==1)
{
   int aux_ultra;
   
   variable_bluetooht=lectura_bluetooht();
   
   if(variable_bluetooht==55)     //cuando BT se desconecta, salgo del While
   modo_bluetooht=0; 

switch (variable_bluetooht) //leo ultrasonido
{
    case 0:  // Hay algo critico a la derecha
    #ifdef debug
    Serial.print("\nBluetooht 0");
    #endif

    movimiento_stop();
    delay(tiempo_stop);
    
    break;
      
    case 1:  // Hay algo critico en el centro
    #ifdef debug
    Serial.print("\nBluetooht 1");
    #endif
   
    aux_ultra=funcion_ultrasonido();
    if((aux_ultra!=0)&&(aux_ultra!=1)&&(aux_ultra!=2))   //me fijo si no hay nada critico en frente
    {
    movimiento_frente();
    delay(tiempo_bluetooht);
    }
   
    break;
    
    case 2:  // Hay algo critico a la izquierda
    #ifdef debug
    Serial.print("\nBluetooht 2");
    #endif

    movimiento_atras();
    delay(tiempo_bluetooht);
    
     break;
  
    case 3:  //todo libre adelante
    #ifdef debug
    Serial.print("\nBluetooht 3");
    #endif

    movimiento_derecha();
    delay(tiempo_bluetooht);
    
      break;
   
    case 4:  //hay algo a la izquierda
    #ifdef debug
    Serial.print("\nBluetooht 4");
    #endif

    movimiento_izquierda();
    delay(tiempo_bluetooht); 

      break;

}
}

 }
}

// ---------------------------------
// definimos librerias de Ultrasonido
//-----------------------------------

int funcion_ultrasonido(void)
{
  lectura_ultrasonido();
   
 if(distancia_der<=distancia_critica)
 return 0;
 if(distancia_centro<=distancia_critica)
 return 1;
 if(distancia_izq<=distancia_critica)
 return 2; 
 
 if((distancia_der>distancia_deteccion)&&(distancia_centro>distancia_deteccion)&&(distancia_izq>distancia_deteccion))
 return 3; 

  if((distancia_der>distancia_deteccion)&&(distancia_centro>distancia_deteccion)&&(distancia_izq<=distancia_deteccion))
 return 4; 

  if((distancia_der>distancia_deteccion)&&(distancia_centro<=distancia_deteccion)&&(distancia_izq>distancia_deteccion))
 return 5; 

  if((distancia_der>distancia_deteccion)&&(distancia_centro<=distancia_deteccion)&&(distancia_izq<=distancia_deteccion))
 return 6; 

  if((distancia_der<=distancia_deteccion)&&(distancia_centro>distancia_deteccion)&&(distancia_izq>distancia_deteccion))
 return 7; 

  if((distancia_der<=distancia_deteccion)&&(distancia_centro>distancia_deteccion)&&(distancia_izq<=distancia_deteccion))
 return 8; 

  if((distancia_der<=distancia_deteccion)&&(distancia_centro<=distancia_deteccion)&&(distancia_izq>distancia_deteccion))
 return 9; 

  if((distancia_der<=distancia_deteccion)&&(distancia_centro<=distancia_deteccion)&&(distancia_izq<=distancia_deteccion))
 return 10; 

 return 11;
 
}

//----------------------------------------------------------------------------------------------------
//                              Funcion Lectura Ultrasonido
//----------------------------------------------------------------------------------------------------

void lectura_ultrasonido(void)
{
   distancia_der=sonar_der.ping_cm();
   distancia_centro=sonar_centro.ping_cm();
   distancia_izq=sonar_izq.ping_cm();

   #ifdef debug
   Serial.print("\nDer= ");
   Serial.print( distancia_der);
   Serial.print("Centro= ");
   Serial.print(distancia_centro);
   Serial.print("Izq= ");
   Serial.print(distancia_izq);
   #endif
   
  if(distancia_der==0)
  distancia_der=distancia_der_anterior;
  else
  distancia_der_anterior=distancia_der;
  
  if(distancia_centro==0)
  distancia_centro=distancia_centro_anterior;
  else
  distancia_centro_anterior=distancia_centro;
  
  if(distancia_izq==0)
  distancia_izq=distancia_izq_anterior;
  else
  distancia_izq_anterior=distancia_izq;

    
   matriz_der[aux_matriz]=distancia_der;
   matriz_centro[aux_matriz]=distancia_centro;
   matriz_izq[aux_matriz]=distancia_izq;

   aux_matriz++;
   if(aux_matriz>=cantidad)
   aux_matriz=0;

   #ifdef debug
    Serial.print("\nAux_matriz= ");
    Serial.print(aux_matriz);

   for(int i=0;i<cantidad;i++)
   {
   Serial.print("\n");
   Serial.print(matriz_der[i]);
   Serial.print("\t");
   Serial.print(matriz_centro[i]);
   Serial.print("\t");
   Serial.print(matriz_izq[i]);
   }
   #endif
   
   distancia_der=0;
   distancia_centro=0;
   distancia_izq=0;
   
   for(int i=0;i<cantidad;i++)
   {
    distancia_der=distancia_der+matriz_der[i];
    distancia_centro=distancia_centro+matriz_centro[i];
    distancia_izq=distancia_izq+matriz_izq[i];
   }
   distancia_der=distancia_der/cantidad;
   distancia_centro=distancia_centro/cantidad;
   distancia_izq=distancia_izq/cantidad;

      #ifdef debug
   Serial.print("\nDer= ");
   Serial.print( distancia_der);
   Serial.print("Centro= ");
   Serial.print(distancia_centro);
   Serial.print("Izq= ");
   Serial.print(distancia_izq);
   #endif
}




// --------------------------------------------------------------------------------
// definimos librerias de movimiento
//---------------------------------------------------------------------------------

void movimiento_stop(void)
{
   motor1.setSpeed(velocidad);  
 motor2.setSpeed(velocidad); 
 motor3.setSpeed(velocidad); 
 motor4.setSpeed(velocidad); 
 
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
   delay(3);
}

// --------------------------------------------------------------------------------

void movimiento_frente(void)
{
   velocidad_atras=velocidad_inicial;
  velocidad_frente=velocidad_frente+velocidad_incremento;
  if(velocidad_frente>255)
  velocidad_frente=255;
 
 motor1.setSpeed(velocidad_frente);  
 motor2.setSpeed(velocidad_frente); 
 motor3.setSpeed(velocidad_frente); 
 motor4.setSpeed(velocidad_frente); 

  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);

   delay(3);
  
}

// --------------------------------------------------------------------------------

void movimiento_atras(void)
{
    velocidad_frente=velocidad_inicial;
  velocidad_atras=velocidad_atras+velocidad_incremento;
    if(velocidad_atras>255)
  velocidad_atras=255;
  
   motor1.setSpeed(velocidad_atras);  
 motor2.setSpeed(velocidad_atras); 
 motor3.setSpeed(velocidad_atras); 
 motor4.setSpeed(velocidad_atras); 

 
   motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);

  
  delay(3);
  
}

// --------------------------------------------------------------------------------

void movimiento_derecha(void)
{
   motor1.setSpeed(velocidad);  
 motor2.setSpeed(velocidad); 
 motor3.setSpeed(velocidad); 
 motor4.setSpeed(velocidad); 

   motor1.run(BACKWARD);
  motor2.run(BACKWARD);
    motor3.run(FORWARD);
  motor4.run(FORWARD);

}

// --------------------------------------------------------------------------------

void movimiento_izquierda(void)
{
   motor1.setSpeed(velocidad);  
 motor2.setSpeed(velocidad); 
 motor3.setSpeed(velocidad); 
 motor4.setSpeed(velocidad); 

    motor1.run(FORWARD);
  motor2.run(FORWARD);
    motor3.run(BACKWARD);
  motor4.run(BACKWARD);
   

}

// --------------------------------------------------------------------------------
//                        Lectura Bluetooht
// --------------------------------------------------------------------------------

int lectura_bluetooht(void)
{
      char c = 'H';                        //variable que voy a utilizar para el bluetooht
      if (BTserial.available())       //si el bluetooht me llama, leo y cargo en c
      c = BTserial.read();

   if((c=='0')||(c=='5')||(c=='6')||(c=='7')||(c=='8')||(c=='9')||(c=='A')) 
   return 0;                         //cualquier comando que recibo mando 0 para avisar que BT conectado
   
   if(c=='1')                        //va para adelante
   return 1; 

   
   if(c=='2')                       //va para atras
   return 2; 

   
   if(c=='3')                       //giro izquierda
   return 3; 

   
    if(c=='4')                   //gira derecha
    return 4; 



   if(c=='H')                    
   return 55;                   //Numero que defino al azar para
                                // indicar que no hay nocexion con BT

}


