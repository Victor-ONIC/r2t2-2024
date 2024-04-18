# r2t2-2024 [Coupe de France de Robotique](https://www.coupederobotique.fr/)

## TODO  
**LiDAR**  
Créer le code LiDAR et l'associer au code de l'Orange Pi.  

**TOFs**  
Mettre les 3 TOFs de proximité sur le robot (Gozlan).  
Gérer les TOFs dans le code de l'Arduino.  

**PAMIs**  
Gérer l'évitement.  
Etablir les stratégies.  

**Robot**  
Comment déposer les plantes empotées dans la jardinière ?  
Etablir des stratégies hardcodées.  
Gérer l'évitement avec les TOFs et le LiDAR.  
Etablir des stratégies dynamiques.  

**Misc**  
Démarrer l'interface graphique au lancement de l'Orange Pi.  
Backup carte SD.  
Voitures personnelles ?  

## Codage de communication:
### Moteurs de mouvement
F - Forwards  
B - Backwards  
R - Rotate  
S - Stop (arrêt d'urgence)  
### Moteurs de pinces
O - Open  
C - Close  
U - Up  
D - Down  
P - Stop (voir Silanus)  
### Spécial
G - Go (signal de départ pour les PAMIs)
T - reseT (voir Silanus)

## Arbre
ROBOT/  
├── + **arduino:** Motion_Control_and_Grippers_Serial.ino    
├── - **arduino TOF:** =(  
├── + **python lidar:** =(  
└── main/  
$\quad$ └── + robot_orders.py   
$\quad$ └── + strategies.py  
$\quad$ └── + **interface graphique orange pi:** main_r2t2.py    
PAMI/    
├── + **vision pami:** UltrasonicSimple_test.ino  
└── + **contrôle pami2:** Xbee_PAM2.ino  
 
 ## Outils
-> **blink** pour le lidar (merci Arthur) https://gitlab.com/sharpattack/blink  
-> **sleepsaver** pour stratégies (merci Arthur) https://gitlab.com/sharpattack/sleepsaver  

## Règlement
[Rediffusion de la présentation](https://www.youtube.com/watch?v=kxao-HGvjS0)  
[Diapositives de la présentation](https://www.coupederobotique.fr/wp-content/uploads/RentreeRobotique_2024.pdf)  
[Règlement Eurobot 2024](https://www.coupederobotique.fr/wp-content/uploads/Eurobot2024_Rules_CUP_FR_FINAL.pdf)  
[Check-list d'homologation](https://www.coupederobotique.fr/wp-content/uploads/E2024_Memento_Homologuation_FR.pdf)  
