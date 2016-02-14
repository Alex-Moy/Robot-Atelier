

//Inclu les libraires
#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

//Définie la vitesse du port série
#define BAUDRATE 19200

//Déclare les valeurs initiales de x
int milx;
int maxx = 0;
int minx = 1024;

//Déclare les valeurs initiales de y
int mily;
int maxy = 0;
int miny = 1024;

//Déclare les valeurs modulables de x et y
long valx;
long valy;

unsigned long valenvoi;
unsigned long valrecept;

//Déclare la libraire ArduinoNunchuk
ArduinoNunchuk nunchuk = ArduinoNunchuk();

void setup()
{
  //Initialise le port série
  Serial.begin(BAUDRATE);

  //Initialise le nunchuk
  nunchuk.init();

  //Définie les valeurs milieux de x et y
  nunchuk.update();
  milx = nunchuk.analogX;
  mily = nunchuk.analogY;

  //Affiche un message dans le moniteur série
  Serial.println("Initialisation du joystick en cour ...");
  Serial.println(' ');

  //Définie les valeurs Min et Max de x et y
  for(nunchuk.update(); nunchuk.cButton == 0 || nunchuk.zButton == 0; nunchuk.update()) {
    maxx = max(maxx, nunchuk.analogX);
    minx = min(minx, nunchuk.analogX);
    maxy = max(maxy, nunchuk.analogY);
    miny = min(miny, nunchuk.analogY);
  }

  //Affiche un message dans le moniteur série
  Serial.println("Initialisation du joystick terminee");
  Serial.println(' ');

  //Affiche un message dans le moniteur série
  Serial.println("Initialisation de la connexion sans fil ...");
  Serial.println(' ');
  
  Mirf.cePin = 8; // CE sur D8
  Mirf.csnPin = 7; // CSN sur D7
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Tout est bon ? Ok let's go !
 
  Mirf.channel = 0; // On va utiliser le canal 0 pour communiquer (128 canaux disponible, de 0 à 127)
  Mirf.payload = sizeof(unsigned long); // = 4, ici il faut déclarer la taille du "payload" soit du message qu'on va transmettre, au max 32 octets
  Mirf.config(); // Tout est bon ? Ok let's go !
   
  Mirf.setTADDR((byte *)"emett"); // Le 1er module va envoyer ses info au 2eme module
  Mirf.setRADDR((byte *)"recep"); // On définit ici l'adresse du 1er module

  //Affiche un message dans le moniteur série
  Serial.println("Connexion sans fil initialisee");
  Serial.println(' ');

  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);
  
  byte valrecept[Mirf.payload];
  byte valenvoi[Mirf.payload];

  Mirf.getData(valrecept);
}

void loop()
{
  miseajour();
  regroupe();
  envoi();
}

void miseajour()
{
  //Serial.println("Message recu, mise à jour ...");
  //Met à jours les valeurs reçue par le joystick
  nunchuk.update();

  //Met à niveau les valeurs reçue par le joystick pour x entre 100 = -100 et 300 = 100
  if(nunchuk.analogX == milx) {valx = 200;}
  else {
    if(nunchuk.analogX < milx) {valx = map(nunchuk.analogX, minx, milx, 100, 200);}
    if(nunchuk.analogX > milx) {valx = map(nunchuk.analogX, milx, maxx, 200, 300);}
  }

  //Met à niveau les valeurs reçue par le joystick pour y entre 100 = -100 et 300 = 100
  if(nunchuk.analogY == mily) {valy = 200;}
  else {
    if(nunchuk.analogY < mily) {valy = map(nunchuk.analogY, miny, mily, 100, 200);}
    if(nunchuk.analogY > mily) {valy = map(nunchuk.analogY, mily, maxy, 200, 300);}
  }
}

void regroupe()
{
  valenvoi = valx * 10000 + valy * 10 + nunchuk.zButton;
  Serial.println(valenvoi);
}

void envoi()
{
  //delay(50);
  //envoi valenvoi
  Mirf.send((byte *)&valenvoi);
  
  //Boucle tant que le message n'a pas été envoyé
  while(Mirf.isSending()){
    //Serial.println("Envoi en cour ...");
  }
  //Serial.println("Message envoye");
  //Serial.println(' ');
}
