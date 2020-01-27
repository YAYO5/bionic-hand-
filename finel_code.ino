#include <Servo.h>
//un flag pour savoir si un ordre a ete lance avec le muscle 1, un autre pour le muscle 2
boolean flagM1 = false;
boolean flagM2 = false;
//nouveau flag pour détecter que la poulie triple est à fond en position ouverte – valeur 1 si position atteinte
boolean taquet = false;
//un flag d’ordre 0=position figee, 1=aller vers, 2=retour position ouverte par défaut
byte ordre = 0;
//des tableaux contenant les positions cibles des servo-moteurs pour chaque mouvement de la librairie
//nota, dans le plan de la main inmoov 5 servos sont orientes avec moteurs a  l’arriere, un avec moteur a l’avant (majeur)
//valeurs pifometriques de developpement pour les angles cibles par doigt et position.
int mainouverte[]={
0,179,0};//rangpositioncible 0
int montredoigt[]={
179,179,179};//rangpositioncible 1
int valise[]={
179,0,179};//rangpositioncible2
int grandepince[]={
150,100,0};//rangpositioncible3
//une variable definissant la position cible. Au depart la position cible est la main ouverte.
int rangpositioncible=0;
//int positioncible[4]; // tres bonne piste mais trop de bugs et pas de temps… a  creuser plus tard (tableau de tableau serait ideal).
//deux capteurs analogiques musculaires permettant d’ecouter les ordres
int M1;  //valeur renvoyee par le premier capteur musculaire (interieur ou fermeture)
int M2;  //valeur renvoyee par le deuxieme capteur musculaire (exterieur ou ouverture)
//valeurs de seuil de detection de contraction musculaire pour chaque capteur
int seuilM1 = 200;
int seuilM2 = 700; //haut ouverture
//5 servo-moteurs pilotes par le systeme et correspondant aux doigts de la main
Servo pouce;
Servo index;
Servo triple;
//variables de stockage de la position de chacun des servos en degres
int pospouce;
int posindex;
int postriple;
//variables pour les angles cibles par doigt en degres
int posciblepouce;
int poscibleindex;
int poscibletriple;
// decommenter si vous souhaitez activer le mode debug sur le port série
// #define DEBUG
void setup()
{
// On attache les servos aux pins de controle.
pouce.attach(0);
index.attach(1);
triple.attach(2);
//un temoin en //Serial
#ifdef DEBUG
Serial.begin(9600);
#endif
//affectation des broches entrees/sorties
//capteurs musculaires sur les analogiques A1 et A2 (la A0 est prise par le LCD shield);
pinMode(A0,INPUT);
pinMode(A1,INPUT);
//on va utiliser la led 13 pour tester
pinMode(13,OUTPUT);
digitalWrite(13,LOW);
//Initialisation de la main – Tres important car va definir la position en degres des servos (90 degres au depart)
initialisationmain();
}
void loop()
{
#ifdef DEBUG
//version de simulation des capteurs avec la frappe de caracteres sur le port serie : a=M1/b=M2/c=double contraction/d=repos des 2 muscles.
while (Serial.available()) {
// get the new byte:
char inChar = (char)Serial.read();
switch(inChar){
//contraction M1
case ‘a’:
M1=200;
M2=50;
break;
//contraction M2;
case ‘b’:
M1=50;
M2=200;
break;
//double contraction
case ‘c’:
M1=200;
M2=200;
break;
//repos
case ‘d’:
M1=50;
M2=50;
break;
}
}*/
#endif
//muscles
M1=analogRead(A0);
M2=analogRead(A1);
//si il y a une alerte musculaire on met a  jour les flags. pour une valeur de 1 en M1 on lance, pour une valeur de zero on stoppe.
if(M1>seuilM1)  {
#ifdef DEBUG
Serial.println(« Contraction muscle 1 »);
#endif
flagM1=1;
}
if(M1<seuilM1)  {
#ifdef DEBUG
Serial.println(« Repos muscle 1 »);
#endif
flagM1=0;
}
if(M2>seuilM2)  {
#ifdef DEBUG
Serial.println(« Contraction muscle 2 »);
#endif
flagM2=1;
}
if(M2<seuilM2)  {
#ifdef DEBUG
Serial.println(« Repos muscle 2 »);
#endif
flagM2=0;
}
//ICI changement.
if(flagM2 && taquet)  {
//si nous sommes arrivé au troisième rang alor il faut retourner au debut
if(rangpositioncible == 3)
rangpositioncible=1;
else
rangpositioncible++;
#ifdef DEBUG
Serial.print(« Changement de position cible, la nouvelle position cible est : »);
Serial.print(rangpositioncible);
Serial.println( » – attente de 1 seconde… »);
#endif
//delay(1000);
//blink(rangpositioncible);
taquet = false;
}
//on reagit a un flag du premier muscle. Le 2eme muscle est prioritaire.
if(flagM1 && !flagM2){
rangpositioncible=2;
mouvement(rangpositioncible,pospouce,posindex,postriple);
}
if(flagM2){
//on va vers main ouverte
rangpositioncible=0;
mouvement(rangpositioncible,pospouce,posindex,postriple);
}
delay(5);
#ifdef DEBUG
temoindebug();
delay(100);
#endif
}
//positionnement initial des moteurs a 90 degres.
void initialisationmain(){
pouce.write(90);
index.write(90);
triple.write(90);
pospouce=90;
posindex=90;
postriple=90;
#ifdef DEBUG
Serial.println(« Initialisation des servos à 90 degres »);
#endif
}
#ifdef DEBUG
void temoindebug(){
Serial.print(« rang de la position a atteindre : « );
Serial.println(rangpositioncible);
Serial.print(« valeur de M1 : « );
Serial.println(M1);
Serial.print(« valeur de M2 : « );
Serial.println(M2);
Serial.print(« valeur de flag M1 : « );
Serial.println(flagM1);
Serial.print(« valeur de flag M2 : « );
Serial.println(flagM2);
Serial.print(« valeur de pouce = « );
Serial.println(pospouce);
Serial.print(« valeur index = « );
Serial.println(posindex);
Serial.print(« valeur triple = « );
Serial.println(postriple);
Serial.print(pospouce);
Serial.print(« , »);
Serial.print(posindex);
Serial.print(« , »);
Serial.println(postriple);
//delay(1000);
}
#endif
void mouvement(int pos,int p, int i, int m){
//on recherche la position cible pour chaque doigt (depend de rangpositioncible)
switch(pos){
case 0:
posciblepouce=mainouverte[0];
poscibleindex=mainouverte[1];
poscibletriple=mainouverte[2];
break;
case 1:
posciblepouce=montredoigt[0];
poscibleindex=montredoigt[1];
poscibletriple=montredoigt[2];
break;
case 2:
posciblepouce=valise[0];
poscibleindex=valise[1];
poscibletriple=valise[2];
break;
case 3:
posciblepouce=grandepince[0];
poscibleindex=grandepince[1];
poscibletriple=grandepince[2];
break;
}
//on incremente ou decremente l’angle de chaque servo-moteur pour chaque doigt (depend de la valeur cible).
// Il ne ne passe rien si les valeurs cibles sont atteintes.
// pouce
if(p<posciblepouce){
pospouce++;
pouce.write(pospouce);
}
if(p>posciblepouce){
pospouce;
pouce.write(pospouce);
}
// index
if(i<poscibleindex){
posindex++;
index.write(posindex);
}
if(i>poscibleindex){
posindex;
index.write(posindex);
}
// triple
if(m<poscibletriple){
postriple++;
triple.write(postriple);
}
if(m>poscibletriple){
postriple;
triple.write(postriple);
}
//si les valeurs cibles sont atteintes suite a un ordre du muscle 2 normalement on conserve la position cible d’origine.
//IMPORTANT : On met un flag pour savoir si la poulie triple est en position étendue à fond.
if(m == poscibletriple && poscibletriple==179) {
#ifdef DEBUG
Serial.println(« Nous sommes au taquet »);
#endif
taquet = true;
}
else
taquet = false;
}
void blink(int z){
for (int r=0;r==z; r++){
digitalWrite(13,HIGH);
delay(500);
digitalWrite(13,LOW);
delay(500);
}
}
