
# Projet-Exp-rience-5-sens-e

Ce dépôt git contient le code du Slave et du Master ain si que les librairies nécéssaires.
#
## Introduction
#
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
### Commande <Col,F1,F2,F3,F4,F5,F6>
#
0=BLEU, 1=VERT, 2=JAUNE, 3=ORANGE, 4=ROUGE, 5=VIOLET, 6=BLANC,  
7=NOIR (NOIR=eteint), 8=BG (BG =couleur BackGround).
On peut ajouter de nouvelles couleurs à la palette.

```
Exemple <Col,4,2,3,4,3,6>
```

#
### Commande  <BGcolor,R,G,B > 
#
Permet de définir la couleur du baground en donnant les quantités de rouge, bleu, et vert ( de 0 à 255 pour chacunes )


```
Exemple <BGcolor,255,0,0> => la couleur BG sera Rouge 
```

#
### Commande  <Animation,Mode,R,G,B,Speed/Vibre>
#
Permet de changer l'animation d'un slave,

- Mode=0 => pas d'animation 
- Mode=1 => rotation 
- Mode=2 => Topface (mode "démo") 
- Mode=3 => FlashAnimation 
- R,G,B => composantes de couleur 
- Speed/Vibre => dans le Mode 1: vitesse de l'animation (de 1 à 1000.  10=rapide, 1000=lent) 
            => dans les autres modes: valide ou non la vibration (0=off, 1=on) lors de la rotation du slave

Par défaut le mode est FlashAnimation

```
Exemple 1: <Animation,1,255,0,0,10> => rotation d'une face Rouge 

Exemple 2: <Animation,2,255,0,0,1> = démo "Topface" en Rouge, avec vibration 

Exemple 3: <Animation,2,0,255,0,0> = démo "Topface" en Vert, sans vibration 

Exemple 4: <Animation,0> => pas d'animation 

Exemple 5: <Animation,3,0,0,0,1> => démo FlashExplode, avec vibration
```

