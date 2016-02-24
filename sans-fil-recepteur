/*
Robot-Atelier
Les programmes pour le robot de l'atelier avec la nunchuck avec et sans fil
Programme par Alexis MOYART
*/

//Inclu les libraires
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

//Définie la vitesse du port série
#define BAUDRATE 19200


//Déclare les valeurs modulables de x et y
long valx;
long valy;

//Déclare la valeur du bouton
int zButton;
int yButton;

//Déclare la variable du mode
int mode = 0;

//Définie le temps pour l'anti-rebont
const int debounce = 500;
long timedebounce;

//Définie la distance limite
const int distancelimite = 40;

//Défnie la valeur d'erreur de distance
const int erreurdistance = 5;

//Définie les pins moteur gauche (à définir)
const int MGpolun = A0;
const int MGpoldeux = A1;
const int MGspeed = 5;

//Définie les pins moteur droite (à définir)
const int MDpolun = A2;
const int MDpoldeux = A3;
const int MDspeed = 3;

//Définie le pin du haut-parleur
const int hp = 4;

//Définie les pins des détecteur de distance
const int trigG = 9;
const int trigD = 2;
const int echoG = 10;
const int echoD = 6;

//Déclare les variable de stockage des vitesses
int vitesseMG;
int vitesseMD;

//Déclare les variables de stockage des mesures
int mesureG;
int mesureD;

unsigned long valrecept;

void setup()
{
  //Initialise le port série
  Serial.begin(BAUDRATE);

  //Déclare les pins du moteur gauche en tant que sortie
  pinMode(MGpolun, OUTPUT);
   pinMode(MGpoldeux, OUTPUT);
    pinMode(MGspeed, OUTPUT);

  //Déclare les pins du moteur droit en tant que sortie
  pinMode(MDpolun, OUTPUT);
   pinMode(MDpoldeux, OUTPUT);
    pinMode(MDspeed, OUTPUT);

  Mirf.cePin = 8; // CE sur D8
  Mirf.csnPin = 7; // CSN sur D7
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Tout est bon ? Ok let's go !
 
  Mirf.channel = 0; // On va utiliser le canal 0 pour communiquer (128 canaux disponible, de 0 à 127)
  Mirf.payload = sizeof(unsigned long); // = 4, ici il faut déclarer la taille du "payload" soit du message qu'on va transmettre, au max 32 octets
  Mirf.config(); // Tout est bon ? Ok let's go !
   
  Mirf.setTADDR((byte *)"recep"); // Le 1er module va envoyer ses info au 2eme module
  Mirf.setRADDR((byte *)"emett"); // On définit ici l'adresse du 1er module

  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);

  Mirf.getData((byte *) &valrecept);

  //delay(500);
}

void loop()
{
  reception();
  degroupe();
  if(mode == 0)
  {
    polaritemoteurcontrole();
    speedmoteurcontrole();
    klaxoncontrole();
  }
  else {
    mesureautonomie();
    actionneautonomie();
  }
}

void reception()
{
  while(!Mirf.dataReady()) {/*Vide*/}
  Mirf.getData((byte *) &valrecept);
  Serial.println(valrecept);
}

void degroupe()
{
  valx = valrecept / 100000;
  valx = valx - 200;
  valy = (valrecept % 100000) / 100;
  valy = valy - 200;
  zButton = (valrecept % 100) / 10;
  yButton = valrecept % 10;
}

void modeselect()
{
  if(zButton == 1 && yButton == 1) {
    if(mode == 0) {mode = 1;}
    if(mode == 1) {mode = 0;}
    delay(1000);
  }
}

void polaritemoteurcontrole()
{
  //Met les deux moteurs en marche arrière
  if(valy < 0) {
    digitalWrite(MGpolun, LOW);
    digitalWrite(MGpoldeux, HIGH);
    digitalWrite(MDpolun, LOW);
    digitalWrite(MDpoldeux, HIGH);
  }

  //Met les deux moteurs en marche avant
  if(valy >= 0) {
    digitalWrite(MGpolun, HIGH);
    digitalWrite(MGpoldeux, LOW);
    digitalWrite(MDpolun, HIGH);
    digitalWrite(MDpoldeux, LOW);
  }
}

void speedmoteurcontrole()
{
  //Pffffiou, là c'est le gros bazard, en gros, je regarde dans quelle zone est je joystick et calcule les vitesse !

  //Zone basse du joystick
  if(valy < -5) {

    //Zone basse gauche du joystick
    if(valx < -5) {
      vitesseMG = map((valy - valx), 0, -100, 0, 255);
      vitesseMD = map(valy, 0, -100, 0, 255);
    }

    //Zone basse centrale du joystick
    if(valx >= -5 && valx <= 5) {
      vitesseMG = map(valy, 0, -100, 0, 255);
      vitesseMD = map(valy, 0, -100, 0, 255);
    }

    //Zone basse droite du joystick
    if(valx > 5) {
      vitesseMG = map(valy, 0, -100, 0, 255);
      vitesseMD = map((valy + valx), 0, -100, 0, 255);
    }
  }

  //Zone de "marge d'erreur" où les moteurs sont éteints
  if(valy >= -5 && valy <= 5) {
    vitesseMG = 0;
    vitesseMD = 0;
  }

  //Zone haute du joystick
  if(valy > 5) {

    //Zone haute gauche du joystick
    if(valx < -5) {
      vitesseMG = map((valy + valx), 0, 100, 0, 255);
      vitesseMD = map(valy, 0, 100, 0, 255);
    }

    //Zone haute centrale du joystick
    if(valx >= -5 && valx <= 5) {
      vitesseMG = map(valy, 0, 100, 0, 255);
      vitesseMD = map(valy, 0, 100, 0, 255);
    }

    //Zone haute droite du joystick
    if(valx > 5) {
      vitesseMG = map(valy, 0, 100, 0, 255);
      vitesseMD = map((valy - valx), 0, 100, 0, 255);
    }
  }

  //Active les moteurs aux bonnes vitesses
  analogWrite(MGspeed, vitesseMG);
  analogWrite(MDspeed, vitesseMD);
}

void klaxoncontrole()
{
  if((millis() - timedebounce) >= debounce) {
    //Fait un double BIP si le bouton Z est préssé
    if(zButton == 1) {
  
     //Fait un premier BIP
      tone(hp, 600, 300);
      delay(300);
      noTone(hp);

      //Attend un petit peu avant le second
      delay(50);

      //Fait un second BIP
      tone(hp, 600, 300);
      delay(300);
      noTone(hp);

      //Réinitialise l'anti-rebont
      timedebounce = millis();
    }
  }
}

void mesureautonomie()
{
  //Mesure à gauche
  digitalWrite(trigG, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigG, LOW);
  mesureG = (pulseIn(echoG, HIGH) / 58,8);

  //Mesure à droite
  digitalWrite(trigD, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigD, LOW);
  mesureD = (pulseIn(echoD, HIGH) / 58,8);
}

void actionneautonomie()
{
  if((mesureG < erreurdistance || mesureG >= distancelimite) && (mesureD < erreurdistance || mesureD >= distancelimite)) {
    arretmoteurs();
    
    //Si les mesures est incorrecte ou que les mesures sont supérieures ou égales à 40cm on allume les moteurs en avant à la vitesse max
    digitalWrite(MGpolun, HIGH);
    digitalWrite(MGpoldeux, LOW);
    digitalWrite(MGspeed, HIGH);
    digitalWrite(MDpolun, HIGH);
    digitalWrite(MDpoldeux, LOW);
    digitalWrite(MGspeed, HIGH);
  }
  
  if(mesureG > erreurdistance && mesureG < distancelimite) {
    arretmoteurs();
    
    //Si il détecte un truc à gauche alors MG avance et MD recule
    digitalWrite(MGpolun, HIGH);
    digitalWrite(MGpoldeux, LOW);
    digitalWrite(MGspeed, HIGH);
    digitalWrite(MDpolun, LOW);
    digitalWrite(MDpoldeux, HIGH);
    digitalWrite(MGspeed, HIGH);
  }
  
  if(mesureD > erreurdistance && mesureD < distancelimite) {
    arretmoteurs();
    
    //Si il détecte un truc à gauche alors MG avance et MD recule
    digitalWrite(MGpolun, LOW);
    digitalWrite(MGpoldeux, HIGH);
    digitalWrite(MGspeed, HIGH);
    digitalWrite(MDpolun, HIGH);
    digitalWrite(MDpoldeux, LOW);
    digitalWrite(MGspeed, HIGH);
  }
}

void arretmoteurs()
{
  //Eteint les moteurs pendant un court temps pour éviter qu'ils repartent imédiatement dans le sens opposé
  digitalWrite(MGspeed, LOW);
  digitalWrite(MGspeed, LOW);
  delay(10);
}
