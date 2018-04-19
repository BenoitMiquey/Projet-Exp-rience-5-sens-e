# Projet-Exp-rience-5-sens-e

Ce dépôt git contient le code du Slave et du Master ain si que les librairies nécéssaires.
#
## Introduction

Important : 
Il y a deux méthodes de communication avec le Slave:
- Par USB 
- Par l'intermédiaire du module radio

La librairie du slave est dans le répertoire lib.

La librairie modifiée rfid est aussi dans le répertoire lib.

Le mode USB est uniquement pour faire des tests directement sur le module slave

Dans le fichier IfDef.h :
-Mettre en commentaire COMUART et DEBUG pour le fonctionement par ondes radio
-Mettre en commentaire juste DEBUG pour communiquer via le cable USB en utilisant le moniteur série
#
## Les commandes envers le module slave
#
### Commande < Col,F1,F2,F3,F4,F5,F6 >
#
0=BLEU, 1=VERT, 2=JAUNE, 3=ORANGE, 4=ROUGE, 5=VIOLET, 6=BLANC,  
7=NOIR (NOIR=eteint), 8=BG (BG =couleur BackGround).
On peut ajouter de nouvelles couleurs à la palette.

```
Exemple <Col,4,2,3,4,3,6>
```

#
### Commande  < BGcolor,R,G,B > 
#
Permet de définir la couleur du baground en donnant les quantités de rouge, bleu, et vert ( de 0 à 255 pour chacunes )


```
Exemple <BGcolor,255,0,0> => la couleur BG sera Rouge 
```

#
### Commande  < Animation,Mode,R,G,B,Speed/Vibre >
#
Permet de changer l'animation d'un slave,

- Mode=0 => pas d'animation 
- Mode=1 => rotation 
- Mode=2 => Topface (mode "démo") 
- Mode=3 => FlashAnimation 
OK,TotState,19,5,0
- R,G,B => composantes de couleur 
- Speed/Vibre => dans le Mode 1: vitesse de l'animation (de 1 à 1000.  10=rapide, 1000=lent) 
            => dans les autres modes: valide ou non la vibration (0=off, 1=on) lors de la rotation du slave

Par défaut le mode est FlashAnimation

```
Exemple 1: <Animation,1,255,0,0,10> => rotation d'une face Rouge 

Exemple 2: <Animation,2,255,0,0,1> = démo "Topface" en Rouge, avec vibration 

Exemple 3: <Animation,2,0,255,0,0> = démo "Topface" en Vert, sans vibration 

Exemple 4: <Animation,0> => pas d'animation 
L
Exemple 5: <Animation,3,0,0,0,1> => démo FlashExplode, avec vibration
```

#
### Commande < FaceLed,Face,Cligno,R,G,B >
#

Permet d'allumer une des 6 faces précise ( de 0 à 5 ) et 6 pour toute les faces 

Cligno: 
- 0: face éteinte (BG color) 
- 1: face allumée sans clignotement 
- supérieur à 1: clignotement vitesse selon la valeur de cligno 

R,G,B => composantes de couleur 


```
Exemple 1 <FaceLed,2,10,255,255,255> => la face 2 clignote en blanc 

Exemple 2 <FaceLed,2,1,255,255,255> => la face 2 allumée en blanc, sans clignotement

Exemple 3 <FaceLed,2,0 > => la face 2 est éteinte (BG color) 

Exemple 4 <FaceLed,6,10,255,0,0> => toutes les faces clignotent en rouge 

```

#
### Commande  < TotState >
#
            
Interroge l'état du Slave, il répondra "OK,TotState,Touch,Secteur,Tilt" 
, Touch= état logique des capteurs tactiles, somme des poids binaires 
Secteur= le N° de la face qui est en haut 
Tilt=l'inclinaison en degres sur l'axe "tangage"

```
Exemples de reponse

"OK,TotState,8,3,0" =>  La face 3 est appuyée, la face du dessus est la face 3

"OK,TotState,19,5,0" => 19 = 16 + 2 +1 => Les faces 4, 1 et 0 sont appuyées et la face 5 est sur le dessus

```

#
### Commande  < TouchState >
#
            
Permet de connaître l'état logique de chaque faces 


```
Exemple de réponse

"OK,TouchState,0,0,1,0,1,0" => Les faces 2 et 4 sont appuyées
```


#
### Commande < TouchLevel >
#
            
Permet de connaître l'état analogique de chaque faces

```
Exemple de réponse 

"OK,TouchLevel,0,0,843,0,941,0" => Touch des faces 2 et 4 activés à 843 et 941 respectivement  
```

#
### Commande < FaceID > 
#
            
Permet de connaître l'dentifiant RFID du slave voisin

```
Exemples de réponse 

"OK,FaceID,0 " => pas de RFID dans le champ de lecture 

"OK,FaceID,2b17e9" => RFID du voisin vaut 2b17e9 
``` 

#
### Commande < Vibre,Durée >
#

Fait vibrer le slave avec une durée, avec 100 qui vaut à peut près une seconde

```
Exemple <Vibre,100> => vibre durant a peu près 1 seconde 
```


#
### Commande < Inclino >
#

Permet de lire les valeurs de l'inclinomètre

```
Exemple de réponse

"OK,Inclino,3,330,0" => Secteur du dessus 3, angle 330 degres, tangage=0 
```


