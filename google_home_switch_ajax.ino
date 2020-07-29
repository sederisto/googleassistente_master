/*
 Version 0.1 - March 17 2018
*/ 
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
//#include <UIPEthernet.h>
#include "DHT.h"
#include "IRremote.h"     //biblioteca para controlar o led infravermelho
#include <Wire.h>

#include <WebSocketsClient.h>    //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h>        // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
//#include <StreamString.h>
#define DHTPIN 2              //onde sensor esta conectado
#define DHTTYPE DHT11        // DHT 11
DHT dht(DHTPIN, DHTTYPE);   //define sensor


#define MyApiKey "bf34cc62-1e4f-42cb-b750-87b124ee5a17" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 
#define SERVER_URL "iot.sinric.com" //"iot.sinric.com"
#define SERVER_PORT 80 // 80

 byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

IPAddress ip(192,168,15,53);
IPAddress gateway(192,168,15,1);
IPAddress subnet(255,255,255,0);
//===================CONECTA AO SERVIDOR NA PORTA ========================================================
EthernetServer server(8081);
//===================Inicio da dista clonagem dos controles remoto========================================  

 uint64_t heartbeatTimestamp = 0;
bool isConnected = false;
WebSocketsClient webSocket;

//#define spot1 30    //relé jardim 
//#define spot2 32    //relé cenario                   
//#define spot3 34    //relé garagem                   
//#define spot4 36    //relé cozinha
//#define spot9 46    //escadaria

//             mensagens dos botoes   { 1           2         3        4           5        6         7          8 
//      CONTROLE - A - TV                
    const   long aparelho1[]            ={0x80c,0x820,0x810,0x0,0x811,0x821,0x801,0x802,0x803,0x804,0x805,0x806,0x807,0x808,0x809,0x838,0x8CA,0x822  };   // codigo dos comandos do aparelho 1 (iniciar com 0x)
    const   int  aparelho1Bits          =12;                                                                                   // frequencia de BITS do aparelho 1
    const   int  aparelho1raw           = 3 ;  // Tipo do comando de envio do aparelho1 1=SONY , 2=NEC , 3=RC5 , 4=RC6
    const   int  tamanho1               = 24;   // Para uso RAW - Tamanho do vetor

//      CONTROLE - B -   HOME    
    const   long aparelho2[]             ={0x40C     ,0x42C    ,0x430    ,0x421     ,0x420    ,0x44E   ,0x44B    ,0x431    };   // codigo dos comandos do aparelho 2 (iniciar com 0x)        
    const   int  aparelho2Bits           =20;                                                                                   // frequencia de BITS do aparelho 2
    const   int  aparelho2raw            = 4 ; // Tipo do comando de envio do aparelho2 1=SONY , 2=NEC , 3=RC5 , 4=RC6                                                                               // Tipo do comando de envio do aparelho 2
    const   int  tamanho2                = 19;   // Para uso RAW - Tamanho do vetor
 
//      CONTROLE - C -  PLAYER  
    const   long aparelho3[]             ={0x9F608F7 ,0x9F6B847,0x9F67887,0x9F638C7 ,0x9F620DF,0x9F68B7,0x9F66897,0x9F658A7};  // codigo dos comandos do aparelho 3 (iniciar com 0x)
    const   int  aparelho3Bits           =32;                                                                                  // frequencia de BITS do aparelho 3
    const   int  aparelho3raw            = 2 ; // Tipo do comando de envio do aparelho3 1=SONY , 2=NEC , 3=RC5 , 4=RC6                                                                              // Tipo do comando de envio do aparelho 3  
    const   int  tamanho3                = 19;   // Para uso RAW - Tamanho do vetor
 
//      CONTROLE - D -  AR CONDICIOANDO  
    const   long aparelho4[]             ={};  // codigo dos comandos do aparelho 4 (iniciar com 0x)
    const   int  aparelho4Bits           =12;                                                                                  // frequencia de BITS do aparelho 4
    const   int  aparelho4raw            = 3 ; // Tipo do comando de envio do aparelho4 1=SONY , 2=NEC , 3=RC5 , 4=RC6                                                                              // Tipo do comando de envio do aparelho 4  
    const   int  tamanho4                = 19;   // Para uso RAW - Tamanho do vetor

//      CONTROLE - E -  CABO 
    const   long aparelho5[]             ={0xFE7887     ,0xFEC03F    ,0xFE02FD    ,0xFEFA05     ,0xFE40BF    ,0x80BF4BB4   ,0x80BF49B6    ,0xD20 , 0xFE7887, 0xFE7887, 0xFE7887, 0xFE7887  };  // codigo dos comandos do aparelho 5 (iniciar com 0x)
    const   int  aparelho5Bits           =32;                                                                                  // frequencia de BITS do aparelho 5
    const   int  aparelho5raw            = 2 ; // Tipo do comando de envio do aparelho5 1=SONY , 2=NEC , 3=RC5 , 4=RC6                                                                              // Tipo do comando de envio do aparelho 5      
    const   int  tamanho5                = 68;   // Para uso RAW - Tamanho do vetor
 
//      CONTROLE - F -   CORTINAS ou JANELAS etc.
    const   long aparelho6[]             ={0xD0C     ,0xD10    ,0xD11    ,0xD39     ,0xD38    ,0xD3A   ,0xD3B    ,0xD3C    };  // codigo dos comandos do aparelho 6 (iniciar com 0x)
    const   int  aparelho6Bits           =12;                                                                                  // frequencia de BITS do aparelho 6
    const   int  aparelho6raw            = 3 ; // Tipo do comando de envio do aparelho6 1=SONY , 2=NEC , 3=RC5 , 4=RC6                                                                              // Tipo do comando de envio do aparelho 6  
    const   int  tamanho6                = 19;   // Para uso RAW - Tamanho do vetor
  
 /*
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       Formato referencial das ações a serem executados de acordo com as mensagem recebidas
       D = "Sistema de Controle Remoto"    
  */
 char msg[6] = "0000#"; // String onde é guardada as msgs recebidas
 IRsend irsend; //variavel para poder enviar o codigo controle remoto
 char posicaoA[]                ={'A','B','C'};
 char posicaoB[]                ={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','Z'};    
 int posicaoArray               =0;  // variavel que irá assumir o valor da posição que está a mensagem recebida, para referenciar o comando.
 
//int buttonState = 0;    //inicia o botão com o status 0
const int ledIR = 9;     //saida pino led infravermelho 
//===============definiçao limite da temperatura=============================
int temperaturamax = 30; //alarme leitura temperatura maxima
int temperaturamin = 17; //alarme leitura temperatura minima
//===========================================================================
// Atribuindo nomes aos pinos de entrada, conectados aos pulsadores
const int pulsa1 = 31;  //Tecla jardim
const int pulsa2 = 33;  //Tecla cenario
const int pulsa3 = 35;  //Tecla garagem
const int pulsa4 = 37;  //Tecla cozinha
const int pulsa5 = 39;  //Tecla wc suite
const int pulsa6 = 41;  //Tecla sala
const int pulsa7 = 43;  //Tecla quarto3
const int pulsa8 = 45;  //Tecla wc superior lavabo
const int pulsa9 = 47;  //Tecla escadaria quartos
const int pulsa10 = 49; //Tecla lavabo inferior
const int pulsa11 = 29; //Tecla quarto2
const int pulsa12 = 27; //Tecla led escadaria em baixo
const int pulsa13 = 25; //Tecla geral valvula hidraulica
const int pulsa14 = 23; //Tecla quarto1 PRINCIPAL
const int pulsa15 = 17; //Tecla atico
const int pulsa16 = 19; //Tecla valvula de gas falta adicionar

// Atribuindo nomes aos pinos de saida, conectados aos relés correspondentes as lampadas
const int spot1 = 30;   //relé jardim 
const int spot2 = 32;   //relé cenario                   
const int spot3 = 34;   //relé garagem                   
const int spot4 = 36;   //relé cozinha
const int spot5 = 38;   //relé wc suite
const int spot6 = 40;   //relé sala
const int spot7 = 42;   //relé quarto3
const int spot8 = 44;   //relé wc superior lavabo 
const int spot9 = 46;   //relé escadaria quartos
const int spot10 = 48;  //relé lavabo inferior
const int spot11 = 28;  //relé quarto2
const int spot12 = 26;  //relé led escadaria em baixo
const int spot13 = 24;  //relé geral valvula hidraulica 
const int spot14 = 22;  //relé quarto1 PRINCIPAL
const int spot15 = 16;  //relé atico
const int spot16 = 18;  //relé valvula de gas 
// Variaveis para armazenar o estado das lampadas
boolean spot1_estado = false;
boolean spot2_estado = false;
boolean spot3_estado = false;
boolean spot4_estado = false;
boolean spot5_estado = false;
boolean spot6_estado = false;
boolean spot7_estado = false;
boolean spot8_estado = false;
boolean spot9_estado = false;
boolean spot10_estado = false;
boolean spot11_estado = false;
boolean spot12_estado = false;
boolean spot13_estado = false;
boolean spot14_estado = false;
boolean spot15_estado = false;
boolean spot16_estado = false;

// Variaveis para armazenar o estado dos pulsadores
boolean pulsa1_estado = false;
boolean pulsa2_estado = false;
boolean pulsa3_estado = false;
boolean pulsa4_estado = false;
boolean pulsa5_estado = false;
boolean pulsa6_estado = false;
boolean pulsa7_estado = false;
boolean pulsa8_estado = false;
boolean pulsa9_estado = false;
boolean pulsa10_estado = false;
boolean pulsa11_estado = false;
boolean pulsa12_estado = false;
boolean pulsa13_estado = false;
boolean pulsa14_estado = false;
boolean pulsa15_estado = false;
boolean pulsa16_estado = false;
//=============================================================================================
void turnOn(String deviceId) {
  if (deviceId == "5f205e8bad7a48327f37bd25") //ID iluminaçao do jardim
  {  
    digitalWrite(spot1, LOW);
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  } 
  else if (deviceId == "5f205eb3ad7a48327f37bd2b") //ID iluminaçao do cenario
  { 
    digitalWrite(spot2, LOW);
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  }
  else if (deviceId == "5f205ee3ad7a48327f37bd33") //ID iluminaçao da garagem
  { 
    digitalWrite(spot3, LOW);
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  }
  else if (deviceId == "5f205efcad7a48327f37bd38") //ID iluminaçao da cozinha
  { 
    digitalWrite(spot4, LOW);
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  }
  else if (deviceId == "5f205f1ead7a48327f37bd3c") //ID iluminaçao da escadaria
  { 
    digitalWrite(spot9, LOW);
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  }
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);    
  }     
}

void turnOff(String deviceId) {
   if (deviceId == "5f205e8bad7a48327f37bd25") //ID iluminaçao do jardim
   {  
     digitalWrite(spot1, HIGH);
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
   }
   else if (deviceId == "5f205eb3ad7a48327f37bd2b") //ID iluminaçao do cenario
   { 
     digitalWrite(spot2, HIGH);
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
  }
  else if (deviceId == "5f205ee3ad7a48327f37bd33") //ID iluminaçao da garagen
   { 
     digitalWrite(spot3, HIGH);
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
  }
  else if (deviceId == "5f205efcad7a48327f37bd38") //ID iluminaçao do jardim
   { 
     digitalWrite(spot4, HIGH);
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
  }
  else if (deviceId == "5f205f1ead7a48327f37bd3c") //ID iluminaçao da escadaria
   { 
     digitalWrite(spot9, HIGH);
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
  }
  else {
     Serial.print("Turn off for unknown device id: ");
     Serial.println(deviceId);    
  }
}
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      //Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      //Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      //Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        //Serial.printf("[WSc] get text: %s\n", payload);
        
        // Example payloads
        // For Switch  types
        // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":true}} // https://developers.google.com/actions/smarthome/traits/onoff
        // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":false}}

//DynamicJsonBuffer jsonBuffer;
       //JsonObject& json = jsonBuffer.parseObject((char*)payload); 
       //String deviceId = json ["deviceId"];     
       //String action = json ["action"];

#if ARDUINOJSON_VERSION_MAJOR == 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char*) payload);      
#endif        
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "action.devices.commands.OnOff") { // Switch 
            String value = json ["value"]["on"];
            Serial.println(value); 
            
            if(value == "true") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      //Serial.printf("[WSc] get binary length: %u\n", length);
      break;
    default: break;
  }
}
//===============================================================================================

// Variavel que controla o tempo até uma detecção confiavel
const int debounceTime = 20;

// Declarando função que detecta por quanto tempo o pulsador foi pressionado;
// pino é o pino sendo verificado;
// &state é o estado do pino (HIGH ou LOW);
// &inicio é quando ele foi pressionado
unsigned long pulsadorTempo(int pino, boolean &state, unsigned long &inicio);

// Variavel para acionar todas as lampadas
boolean todas = false;

//Teste toques multiplos
int contagem=0;
long tempoA=0;

char incomingByte[6];
int inchar = 0;

void setup() {
  
  Serial.begin(9600);
  Wire.begin();
  dht.begin();        
  Serial.println("Iniciando programa..");
  Ethernet.begin(mac,ip,gateway,subnet);
  Serial.println("Server ativo no IP: ");
  Serial.print(Ethernet.localIP());
  Serial.println();
  server.begin();
  //endereço servidor sinric, e porta
  webSocket.begin(SERVER_URL, SERVER_PORT, "/");
  //webSocket.begin("iot.sinric.com", 80, "/"); //"iot.sinric.com", 80

  //manipulador de eventos
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  //tente novamente a cada 5000ms se a conexão falhar
  webSocket.setReconnectInterval(5000);   //If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets

  //led infravermelho conectado      
  pinMode(ledIR,  OUTPUT); // pino 9 digital de saída do infravermelho 
 
//Definindo os pinos conectados aos pulsadores como entrada
  pinMode(pulsa1,INPUT);
  pinMode(pulsa2,INPUT);
  pinMode(pulsa3,INPUT);
  pinMode(pulsa4,INPUT);
  pinMode(pulsa5,INPUT);
  pinMode(pulsa6,INPUT);
  pinMode(pulsa7,INPUT);
  pinMode(pulsa8,INPUT);
  pinMode(pulsa9,INPUT);
  pinMode(pulsa10,INPUT);
  pinMode(pulsa11,INPUT);
  pinMode(pulsa12,INPUT);
  pinMode(pulsa13,INPUT);
  pinMode(pulsa14,INPUT);
  pinMode(pulsa15,INPUT);
  pinMode(pulsa16,INPUT); 
  //Ativando o resistor PULL-UP
  digitalWrite(pulsa1,HIGH);
  digitalWrite(pulsa2,HIGH);
  digitalWrite(pulsa3,HIGH);
  digitalWrite(pulsa4,HIGH);
  digitalWrite(pulsa5,HIGH);
  digitalWrite(pulsa6,HIGH);
  digitalWrite(pulsa7,HIGH);
  digitalWrite(pulsa8,HIGH);
  digitalWrite(pulsa9,HIGH);
  digitalWrite(pulsa10,HIGH);
  digitalWrite(pulsa11,HIGH);
  digitalWrite(pulsa12,HIGH);
  digitalWrite(pulsa13,HIGH);
  digitalWrite(pulsa14,HIGH);
  digitalWrite(pulsa15,HIGH);
  digitalWrite(pulsa16,HIGH);
//Definindo os pinos conectados aos relés como saida
  pinMode(spot1,OUTPUT);
  pinMode(spot2,OUTPUT);
  pinMode(spot3,OUTPUT);
  pinMode(spot4,OUTPUT);
  pinMode(spot5,OUTPUT);
  pinMode(spot6,OUTPUT);
  pinMode(spot7,OUTPUT);
  pinMode(spot8,OUTPUT);
  pinMode(spot9,OUTPUT);
  pinMode(spot10,OUTPUT);
  pinMode(spot11,OUTPUT);
  pinMode(spot12,OUTPUT);
  pinMode(spot13,OUTPUT);
  pinMode(spot14,OUTPUT);
  pinMode(spot15,OUTPUT);
  pinMode(spot16,OUTPUT); 
//Escreve na eeprom as saidas, no caso falta de energia executa uma vez... 
  digitalWrite(spot1,EEPROM.read(0));        
  digitalWrite(spot2,EEPROM.read(1));        
  digitalWrite(spot3,EEPROM.read(2));        
  digitalWrite(spot4,EEPROM.read(3));  
  digitalWrite(spot5,EEPROM.read(4)); 
  digitalWrite(spot6,EEPROM.read(5));        
  digitalWrite(spot7,EEPROM.read(6));        
  digitalWrite(spot8,EEPROM.read(7));        
  digitalWrite(spot9,EEPROM.read(8));  
  digitalWrite(spot10,EEPROM.read(9));
  digitalWrite(spot11,EEPROM.read(10)); 
  digitalWrite(spot12,EEPROM.read(11));
  digitalWrite(spot13,EEPROM.read(12));
  digitalWrite(spot14,EEPROM.read(13));
  digitalWrite(spot15,EEPROM.read(14));   
  digitalWrite(spot16,EEPROM.read(15));

}


void loop() {
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }
EthernetClient client = server.available();  
   
delay(100);
//=========================INICIO PULSADORES AVANÇADO=========================================================  
//============================pulso spot1=====================================================================
    int tempoPulsa1 = pulsador1Tempo();
    
    if (tempoPulsa1 > 1500){
      if(!todas){
        int i = 0;
        for (i=0;i<4;i++){
          tone(13, 800, 50); // play the tone
          delay(100);
        }
      }
      todas=true;
    }
    else if (tempoPulsa1 > debounceTime){
      pulsa1_estado = true;
    }
    else {
      if (todas){
        aciona_todas();
        pulsa1_estado = false;
        todas = false;
      }
      else if (pulsa1_estado == true ) {
        //comutaRele(spot1, spot1_estado);
        pulsa1_estado = false;
        contagem++;
        tempoA=millis();
      }
      if (contagem > 0){
        if(millis() - tempoA > 200){
          if(contagem==1)
            comutaRele(spot1,spot1_estado);
          else if (contagem == 2){
            comutaRele(spot1,spot1_estado);
            tone(13, 800, 50);
          }
          contagem=0;
        }
      }
    }
   //============================pulso spot2===================================
    int tempoPulsa2 = pulsador2Tempo();
    if (tempoPulsa2 > debounceTime){
      pulsa2_estado = true;
    }
    
    else {
      if (pulsa2_estado == true ) {
        comutaRele(spot2, spot2_estado);
        pulsa2_estado = false;
      }
    }
  //============================pulso spot3=========================================== 
    int tempoPulsa3 = pulsador3Tempo();
    if (tempoPulsa3 > debounceTime){
      pulsa3_estado = true;
    }
    
    else {
      if (pulsa3_estado == true ) {
        comutaRele(spot3, spot3_estado);
        pulsa3_estado = false;
      }
   }
   //============================pulso spot4=========================================== 
    int tempoPulsa4 = pulsador4Tempo();
    if (tempoPulsa4 > debounceTime){
      pulsa4_estado = true;
    }
    
    else {
      if (pulsa4_estado == true ) {
        comutaRele(spot4, spot4_estado);
        pulsa4_estado = false;
      }
   }
   //============================pulso spot5=========================================== 
    int tempoPulsa5 = pulsador5Tempo();
    if (tempoPulsa5 > debounceTime){
      pulsa5_estado = true;
    }
    
    else {
      if (pulsa5_estado == true ) {
        comutaRele(spot5, spot5_estado);
        pulsa5_estado = false;
      }
   }
   //============================pulso spot6=========================================== 
    int tempoPulsa6 = pulsador6Tempo();
    if (tempoPulsa6 > debounceTime){
      pulsa6_estado = true;
    }
    
    else {
      if (pulsa6_estado == true ) {
        comutaRele(spot6, spot6_estado);
        pulsa6_estado = false;
      }
   }
  //============================pulso spot7=========================================== 
    int tempoPulsa7 = pulsador7Tempo();
    if (tempoPulsa7 > debounceTime){
      pulsa7_estado = true;
    }
    
    else {
      if (pulsa7_estado == true ) {
        comutaRele(spot7, spot7_estado);
        pulsa7_estado = false;
      }
   }
   //============================pulso spot8=========================================== 
    int tempoPulsa8 = pulsador8Tempo();
    if (tempoPulsa8 > debounceTime){
      pulsa8_estado = true;
    }
    
    else {
      if (pulsa8_estado == true ) {
        comutaRele(spot8, spot8_estado);
        pulsa8_estado = false;
      }
   }
   //============================pulso spot9=========================================== 
    int tempoPulsa9 = pulsador9Tempo();
    if (tempoPulsa9 > debounceTime){
      pulsa9_estado = true;
    }
    
    else {
      if (pulsa9_estado == true ) {
        comutaRele(spot9, spot9_estado);
        pulsa9_estado = false;
      }
   }
   //============================pulso spot10=========================================== 
    int tempoPulsa10 = pulsador10Tempo();
    if (tempoPulsa10 > debounceTime){
      pulsa10_estado = true;
    }
    
    else {
      if (pulsa10_estado == true ) {
        comutaRele(spot10, spot10_estado);
        pulsa10_estado = false;
      }
   }
   //============================pulso spot11=========================================== 
    int tempoPulsa11 = pulsador11Tempo();
    if (tempoPulsa11 > debounceTime){
      pulsa11_estado = true;
    }
    
    else {
      if (pulsa11_estado == true ) {
        comutaRele(spot11, spot11_estado);
        pulsa11_estado = false;
      }
   }
  //============================pulso spot12=========================================== 
    int tempoPulsa12 = pulsador12Tempo();
    if (tempoPulsa12 > debounceTime){
      pulsa12_estado = true;
    }
    
    else {
      if (pulsa12_estado == true ) {
        comutaRele(spot12, spot12_estado);
        pulsa12_estado = false;
      }
   }
 //============================pulso spot13=========================================== 
    int tempoPulsa13 = pulsador13Tempo();
    if (tempoPulsa13 > debounceTime){
      pulsa13_estado = true;
    }
    
    else {
      if (pulsa13_estado == true ) {
        comutaRele(spot13, spot13_estado);
        pulsa13_estado = false;
      }
   }
//============================pulso spot14=========================================== 
    int tempoPulsa14 = pulsador14Tempo();
    if (tempoPulsa14 > debounceTime){
      pulsa14_estado = true;
    }
    
    else {
      if (pulsa14_estado == true ) {
        comutaRele(spot14, spot14_estado);
        pulsa14_estado = false;
      }
   }
//============================pulso spot15=========================================== 
    int tempoPulsa15 = pulsador15Tempo();
    if (tempoPulsa15 > debounceTime){
      pulsa15_estado = true;
    }
    
    else {
      if (pulsa15_estado == true ) {
        comutaRele(spot15, spot15_estado);
        pulsa15_estado = false;
      }
   }
//============================pulso spot16=========================================== 
    int tempoPulsa16 = pulsador16Tempo();
    if (tempoPulsa16 > debounceTime){
      pulsa16_estado = true;
    }
    
    else {
      if (pulsa16_estado == true ) {
        comutaRele(spot16, spot16_estado);
        pulsa16_estado = false;
      }
   }   

///////////////////Final do codigo pulsa aqui//////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////
           
//delay(1);

  if(client)
  { 
    boolean continua = true;
    String linha = "";

    while(client.connected())
    {
      if(client.available()){
        char c = client.read();
        linha.concat(c);
  
        if(c == '\n' && continua)
        {
          
          client.println("HTTP/1.1 200 OK");
          // IMPORTANTE, ISSO FAZ O ARDUINO RECEBER REQUISIÇÃO AJAX DE OUTRO SERVIDOR E NÃO APENAS LOCAL.
          client.println("Content-Type: text/javascript");
          client.println("Access-Control-Allow-Origin: *");
          client.println();      
     
          int iniciofrente = linha.indexOf("?");
          
          if(iniciofrente>-1){     //verifica se o comando veio
            iniciofrente     = iniciofrente+6; //pega o caractere seguinte
            int fimfrente    = iniciofrente+4; //esse comando espero 3 caracteres
            String acao    = linha.substring(iniciofrente,fimfrente);//recupero o valor do comando
            
          
            if      ( acao == "001%"){ digitalWrite(spot1, LOW); } //cozinha
            else if ( acao == "002%"){ digitalWrite(spot1, HIGH);} 
            
            else if ( acao == "003%"){ digitalWrite(spot2, LOW); }//sala
            else if ( acao == "004%"){ digitalWrite(spot2, HIGH); }
            
            else if ( acao == "005%"){ digitalWrite(spot3, LOW); }// garagem
            else if ( acao == "006%"){ digitalWrite(spot3, HIGH); }
            
            else if ( acao == "007%"){ digitalWrite(spot4, LOW); }//escada teto
            else if ( acao == "008%"){ digitalWrite(spot4, HIGH); }
            
            else if ( acao == "009%"){ digitalWrite(spot5, LOW); }
            else if ( acao == "010%"){ digitalWrite(spot5, HIGH); }
            
            else if ( acao == "011%"){ digitalWrite(spot6, LOW); }
            else if ( acao == "012%"){ digitalWrite(spot6, HIGH); }
         
            else if ( acao == "013%"){ digitalWrite(spot7, LOW); }
            else if ( acao == "014%"){ digitalWrite(spot7, HIGH); }
            
            else if ( acao == "015%"){ digitalWrite(spot8, LOW); }
            else if ( acao == "016%"){ digitalWrite(spot8, HIGH); }
            
            else if ( acao == "017%"){ digitalWrite(spot9, LOW); }
            else if ( acao == "018%"){ digitalWrite(spot9, HIGH); }
            
            else if ( acao == "019%"){ digitalWrite(spot10, LOW); }
            else if ( acao == "020%"){ digitalWrite(spot10, HIGH); }
            
            else if ( acao == "021%"){ digitalWrite(spot11, LOW); }
            else if ( acao == "022%"){ digitalWrite(spot11, HIGH); }
            
            else if ( acao == "023%"){ digitalWrite(spot12, LOW); }//valvula de gas
            else if ( acao == "024%"){ digitalWrite(spot12, HIGH); }
            
            else if ( acao == "025%"){ digitalWrite(spot13, LOW); }//valvula hidraulica geral
            else if ( acao == "026%"){ digitalWrite(spot13, HIGH); }
            
            else if ( acao == "027%"){ digitalWrite(spot14, LOW); }
            else if ( acao == "028%"){ digitalWrite(spot14, HIGH); }
            
            else if ( acao == "029%"){ digitalWrite(spot15, LOW); }
            else if ( acao == "030%"){ digitalWrite(spot15, HIGH); }
            
            else if ( acao == "031%"){ digitalWrite(spot16, LOW); }
            else if ( acao == "032%"){ digitalWrite(spot16, HIGH); }
                                             
            else if(acao.indexOf('#')){
              
                  ///CONTROLE REMOTO
                  msg[0]=msg[1];
                  msg[1]=msg[2];
            msg[2]=msg[3];
                  acao.toCharArray(msg,6);
                 
                  if (msg[3]=='%') { 
                     if(msg[0] == '0'){
                       posicaoArray = msg[1]-'0';          //00E# converte string em decimal
                     }else{
                       posicaoArray = (msg[0]+msg[1])-'0';//00E#
                     }
                    
                     switch(msg[2])
                     {   // CASO A MENSAGEM SEJA PARA A APARELHO1
                         case'A': enviaIR(aparelho1raw,aparelho1[posicaoArray],aparelho1Bits);break; 
                         
                         // CASO A MENSAGEM SEJA PARA APARELHO2
                         case'B': enviaIR(aparelho2raw,aparelho2[posicaoArray],aparelho2Bits);break; 
                         
                         // CASO A MENSAGEM SEJA PARA APARELHO3
                         case'C':enviaIR(aparelho3raw,aparelho3[posicaoArray],aparelho3Bits);break;  
                         
                         // CASO A MENSAGEM SEJA PARA APARELHO4               
                         case'D':enviaIR(aparelho4raw,aparelho4[posicaoArray],aparelho4Bits);break;
                         
                         // CASO A MENSAGEM SEJA PARA APARELHO5               
                         case'E': 
                             Serial.println(aparelho5[posicaoArray]); 
                             if(aparelho5raw == 5){
                               
                             }else
                                 enviaIR(aparelho5raw,aparelho5[posicaoArray],aparelho5Bits);break;
                         // CASO A MENSAGEM SEJA PARA APARELHO6               
                         case'F':enviaIR(aparelho6raw,aparelho6[posicaoArray],aparelho6Bits);break;                                                            
                      }// fim Switch msg[1]           
                 }//fim if
            }
     
            else {}
            
            float h = dht.readHumidity();
            float t = dht.readTemperature();
            
//=========================mostra temperatura ou se habilitar para ligar  ar condicionado============================================          
            client.print("{");   
            
            client.print("'sensor1': "); 
            client.print(t); 
            
            client.print(", 'sensor2': "); 
            client.print(h);          
                       
                  if(t >= temperaturamax)          //int temperaturamax = 27;
                         {                        

                         client.print(", 'Status': ");
                         //client.print("'RESFRIAR'");
                          }
                    
                   if(t <= temperaturamin)        //int temperaturamin = 21;
                         {
                          //digitalWrite(aquece, LOW);
                          client.print(", 'Status': ''");
                          //client.print("'AQUECER'");
                         }
                         
//==================/////STATUS PULSADOR AVANÇADO e SENSOR////////////==========================
     
            client.print(", 'pulsa1': ");
            client.print(digitalRead(spot1));            

            client.print(", 'pulsa2': ");
            client.print(digitalRead(spot2));            

            client.print(", 'pulsa3': ");
            client.print(digitalRead(spot3));            

            client.print(", 'pulsa4': ");
            client.print(digitalRead(spot4));            

            client.print(", 'pulsa5': ");
            client.print(digitalRead(spot5));            

            client.print(", 'pulsa6': ");
            client.print(digitalRead(spot6));            

            client.print(", 'pulsa7': ");
            client.print(digitalRead(spot7));            

            client.print(", 'pulsa8': ");
            client.print(digitalRead(spot8));
            
            client.print(", 'pulsa9': ");
            client.print(digitalRead(spot9));            

            client.print(", 'pulsa10': ");
            client.print(digitalRead(spot10));            
         
            client.print(", 'pulsa11': ");
            client.print(digitalRead(spot11));            

            client.print(", 'pulsa12': ");
            client.print(digitalRead(spot12));
            
            client.print(", 'pulsa13': ");
            client.print(digitalRead(spot13));
            
            client.print(", 'pulsa14': ");
            client.print(digitalRead(spot14));
             
            client.print(", 'pulsa15': ");
            client.print(digitalRead(spot15));
            
            client.print(", 'pulsa16': ");
            client.print(digitalRead(spot16));
            
            client.print("}");
  
         }          
          break;
        }
        if(c == '\n') { continua = true; }
        else if (c != '\r') { continua = false; }
      }
    }
  } 

/*
    ***************************************************************************************************************************************
    *******************************  Grava informações na EEprom para Backup em caso de queda de energia.. ********************************
    ***************************************************************************************************************************************
*/
     EEPROM.write(0,digitalRead(spot1));              
     EEPROM.write(1,digitalRead(spot2));              
     EEPROM.write(2,digitalRead(spot3));              
     EEPROM.write(3,digitalRead(spot4));              
     EEPROM.write(4,digitalRead(spot5));
     EEPROM.write(5,digitalRead(spot6));              
     EEPROM.write(6,digitalRead(spot7));              
     EEPROM.write(7,digitalRead(spot8));              
     EEPROM.write(8,digitalRead(spot9));              
     EEPROM.write(9,digitalRead(spot10)); 
     EEPROM.write(10,digitalRead(spot11));
     EEPROM.write(11,digitalRead(spot12));
     EEPROM.write(12,digitalRead(spot13));
     EEPROM.write(13,digitalRead(spot14));
     EEPROM.write(14,digitalRead(spot15));
     EEPROM.write(15,digitalRead(spot16));
     delay(10);
     client.stop();
 
  }//FIM DO LOOP AQUI!!!!!!!     
  
}
//Função para detectar por quanto tempo o pulsador foi pressionado
unsigned long pulsadorTempo(int pino, boolean &state, unsigned long &inicio){
  if(digitalRead(pino) != state) {
    state = !state;
    inicio = millis();
  }
  if (state == LOW)
    return millis() - inicio;
  else
    return 0;
}
//Funçoes abaixo sao especificas de cada pulsador  
long pulsador1Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa1, state, inicio);
}

long pulsador2Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa2, state, inicio);
}

long pulsador3Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa3, state, inicio);
}

long pulsador4Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa4, state, inicio);
}

long pulsador5Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa5, state, inicio);
}

long pulsador6Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa6, state, inicio);
}

long pulsador7Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa7, state, inicio);
}

long pulsador8Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa8, state, inicio);
}

long pulsador9Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa9, state, inicio);
}

long pulsador10Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa10, state, inicio);
}

long pulsador11Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa11, state, inicio);
}

long pulsador12Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa12, state, inicio);
}

long pulsador13Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa13, state, inicio);
}

long pulsador14Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa14, state, inicio);
}

long pulsador15Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa15, state, inicio);
}

long pulsador16Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa16, state, inicio);
}

//Funçao que comuta o rele - debauce acionado ligar spot '2 4 e 6' para ligar mais lampadas adicionar nesta funçao;
void comutaRele(int pino, boolean &estado){
  estado = !estado;
  digitalWrite(pino, estado);
  Serial.println("Mudou");
}

void aciona_todas(){                    
  if (!spot2_estado){                  //spot cenario sala
    spot2_estado = !spot2_estado;
    digitalWrite(spot2, spot2_estado);
  }
  if (!spot4_estado){                 //spot Cozinha 
    spot4_estado = !spot4_estado;
    digitalWrite(spot4, spot4_estado);
  }
  if (!spot6_estado){                 //spot Sala
    spot6_estado = !spot6_estado;
    digitalWrite(spot6, spot6_estado);
  }

}

/*
    ***************************************************************************************************************************************
    ***  função para percorrer a variavel de acordo com a mensagem enviada para saber qual a posição que está sendo referenciada. *********
    ***************************************************************************************************************************************
*/
   int percorreArray(char pos1, char pos2)
   {
     posicaoArray = 0;
     int x = 0;
            for(int i = 0; i < sizeof(posicaoA);i++)
            {
              if(pos1 == posicaoA[i])
              {
               for(int z = 0; z < sizeof(posicaoB);z++)
               {
                 if(pos2 == posicaoB[z])
                 {
                   if(pos1=='B')
                   {x = z + 24;}else x = z;
                   break;
                 }
               }
              }
            }
          return x;
   }

    /*
    ***************************************************************************************************************************************
    *********************  função para identificar tipo de código e enviar frequencia para o LED IR   *************************************
    ***************************************************************************************************************************************
   */
    void enviaIR ( int codigo, long mensagem, int bits )
   {
                                 
                    if(codigo == 1)
                    {
                         digitalWrite(4,LOW);
                           for (int s = 0; s < 3; s++) 
                              {
                                 irsend.sendSony(mensagem,bits); 
                                 delay(50);
                              }
                    }
                    if(codigo == 2)
                    {
                      
                           for (int s = 0; s < 1; s++) 
                              {
                                  irsend.sendNEC(mensagem,bits); 
                                  delay(100);
                               }

                     }
                    if(codigo == 3)
                    {
                           for (int s = 0; s < 1; s++) 
                              {
                                  irsend.sendRC5(mensagem,bits); 
                                  delay(100);
                               }
                   }
                    if(codigo == 4)
                    {
                        digitalWrite(4,HIGH);
                           for (int s = 0; s < 2; s++) 
                              {
                                  irsend.sendRC6(mensagem,bits); 
                                  delay(100);
                               }
                   }
       
  }

 
