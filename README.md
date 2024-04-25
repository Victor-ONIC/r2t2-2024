# r2t2-2024 [Coupe de France de Robotique](https://www.coupederobotique.fr/)

## Documents  
[**DRIVE**](https://drive.google.com/drive/folders/1lmPjgmmsK5gpW3CfHZnVjKZY5H6BX998?usp=drive_link)  
[Document Dernière ligne droite avec TODO](https://docs.google.com/document/d/1ib2nAN-OqedOsTI4uglDpM-oiN0k7QHGgBVB4PICick/edit?usp=sharing)  
[Inventaire](https://docs.google.com/document/d/1f4gSAfUIL4cjW8MNNj-gpVyq1s6R6B_Ud4t1gR1wwBo/edit?usp=sharing)  
[Pseudoschéma](https://drive.google.com/file/d/1sg80vgQ7ksRsovvcUBGhDWCgdTUNw8Cg/view?usp=drive_link) à ouvrir dans https://app.diagrams.net/

## Règlement
[Règlement Eurobot 2024](https://www.coupederobotique.fr/wp-content/uploads/Eurobot2024_Rules_CUP_FR_FINAL.pdf)  
[Check-list d'homologation](https://www.coupederobotique.fr/wp-content/uploads/E2024_Memento_Homologuation_FR.pdf)  
[Rediffusion de la présentation](https://www.youtube.com/watch?v=kxao-HGvjS0)  
[Diapositives de la présentation](https://www.coupederobotique.fr/wp-content/uploads/RentreeRobotique_2024.pdf)  

## Code
ROBOT/  
├── **démarrage automatique**: r2t2.desktop ( *~/.config/autostart*)  
├── **arduino mega:** Motion_Control_and_Grippers_Serial.ino    
├── **arduino uno lecture des TOFs:** tof.ino  
└── **Orange Pi:** main/  
&emsp; ├── **com série orangepi <-> arduino mega:** CCommunication2.py  
&emsp; ├── **API ordres:** robot_orders.py   
&emsp; ├── **stockage stratégies:** strategies.py  
&emsp; ├── **écran d'accueil:** main_r2t2.py    
&emsp; ├── main_r2t2_support.py  
&emsp; ├── **écran test:** test2.py  
&emsp; ├── test2_support.py  
&emsp; ├── **écran choix équipe:** app_run.py  
&emsp; ├── **écrans équipes:** app_run_support.py  
PAMI/    
├── **arduino nano? lecture capteur:** UltrasonicSimple_test_FP  
├── **arduino uno PAMI Zone L1/R1:** Xbee_PAMI_V2_PL_1.ino  
└── **arduino uno PAMI Zone L3/R3:** Xbee_PAMI_V2_PL_3.ino  

## Ordres de communication:
F - Forwards  
B - Backwards  
R - Rotate  
S - Stop  
T - reseT  

O - Open  
C - Close  
U - Up  
D - Down  

P - Points  

g - PAMI  
d - PAMI  

 ## Outils
-> **blink** pour le lidar (merci Arthur) https://gitlab.com/sharpattack/blink  
-> **sleepsaver** pour stratégies (merci Arthur) https://gitlab.com/sharpattack/sleepsaver  
