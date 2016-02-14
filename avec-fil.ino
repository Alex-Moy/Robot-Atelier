/*
Robot-Atelier
Les programmes pour le robot de l'atelier avec la nunchuck avec et sans fil
Programme par Alexis MOYART
*/

//Inclu les libraires
#include <Wire.h>
#include <ArduinoNunchuk.h>

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
int valx;
int valy;

//Définie les pins moteur gauche (à définir)
const int MGpolun = 10;
const int MGpoldeux = 9 ;
const int MGspeed = 5;

//Définie les pins moteur droite (à définir)
const int MDpolun = 11;
const int MDpoldeux = 12;
const int MDspeed = 6;

const int hp = 8;

//Déclare les variable de stockage des vitesses
int vitesseMG;
int vitesseMD;

//Déclare la libraire ArduinoNunchuk
ArduinoNunchuk nunchuk = ArduinoNunchuk();

void setup()
{
  //Initialise le port série
  Serial.begin(BAUDRATE);

  //Initialise le nunchuk
  nunchuk.init();

  //Déclare les pins du moteur gauche en tant que sortie
  pinMode(MGpolun, OUTPUT);
   pinMode(MGpoldeux, OUTPUT);
    pinMode(MGspeed, OUTPUT);

  //Déclare les pins du moteur droit en tant que sortie
  pinMode(MDpolun, OUTPUT);
   pinMode(MDpoldeux, OUTPUT);
    pinMode(MDspeed, OUTPUT);

  //S'assure que les moteurs soit éteint
  analogWrite(MGspeed, vitesseMG);
  analogWrite(MDspeed, vitesseMD);

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
}

void loop()
{
  miseajour();
  polaritemoteur();
  speedmoteur();
  klaxon();
}

void miseajour()
{
  //Met à jours les valeurs reçue par le joystick
  nunchuk.update();

  //Met à niveau les valeurs reçue par le joystick pour x
  if(nunchuk.analogX == milx) {valx = 0;}
  else {
    if(nunchuk.analogX > milx) {valx = map(nunchuk.analogX, milx, maxx, 0, 100);}
    if(nunchuk.analogX < milx) {valx = map(nunchuk.analogX, minx, milx, -100, 0);}
  }

  //Met à niveau les valeurs reçue par le joystick pour y
  if(nunchuk.analogY == mily) {valx = 0;}
  else {
    if(nunchuk.analogY > mily) {valy = map(nunchuk.analogY, mily, maxy, 0, 100);}
    if(nunchuk.analogY < mily) {valy = map(nunchuk.analogY, miny, mily, -100, 0);}
  }
}

void polaritemoteur()
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

void speedmoteur()
{
  //Pffffiou, là c'est le gros bazard, en gros, je regarde dans quelle zone est je joystick et calcule les vitesse !

  //Zone basse du joystick
  if(valy < -5) {

    //Zone basse gauche du joystick
    if(valx < -10) {
      vitesseMG = map((valy - valx), 0, -100, 0, 255);
      vitesseMD = map(valy, 0, -100, 0, 255);
    }

    //Zone basse centrale du joystick
    if(valx >= -10 && valx <= 10) {
      vitesseMG = map(valy, 0, -100, 0, 255);
      vitesseMD = map(valy, 0, -100, 0, 255);
    }

    //Zone basse droite du joystick
    if(valx > 10) {
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
    if(valx < -10) {
      vitesseMG = map((valy + valx), 0, 100, 0, 255);
      vitesseMD = map(valy, 0, 100, 0, 255);
    }

    //Zone haute centrale du joystick
    if(valx >= -10 && valx <= 10) {
      vitesseMG = map(valy, 0, 100, 0, 255);
      vitesseMD = map(valy, 0, 100, 0, 255);
    }

    //Zone haute droite du joystick
    if(valx > 10) {
      vitesseMG = map(valy, 0, 100, 0, 255);
      vitesseMD = map((valy - valx), 0, 100, 0, 255);
    }
  }

  //Active les moteurs aux bonnes vitesses
  analogWrite(MGspeed, vitesseMG);
  analogWrite(MDspeed, vitesseMD);
}

void klaxon()
{
  //Fait un double BIP si le bouton Z est préssé
  if(nunchuk.zButton == 1) {

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
  }
}
