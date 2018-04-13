# Projet-Exp-rience-5-sens-e

Ce dépôt git contient le code du Slave et du Master.
#
# Introduction
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
# Les commandes envers le module slave
#
Commande <Col,F1,F2,F3,F4,F5,F6>

permet d'affecter une couleur à chaques faces.
- 0=BLEU
- 1=VERT
- 2=JAUNE
- 3=ORANGE
- 4=ROUGE
- 5=VIOLET
- 6=BLANC 
- 7=NOIR
- (NOIR=eteint)
- 8=BG (BG =couleur BackGround) 
