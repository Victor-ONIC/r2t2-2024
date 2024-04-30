
from robot_orders import *

STRATEGY_1B = [
    claw("C", "Y"),
    
    # Panneau 1
    move("B", "X", 50),
    rotate(10),
    move("F", "Y", 105),
    point("P", 5),
    move("B", "Y", 105),
    rotate(-10),

    # Panneau 2
    move("B", "X", 225),
    rotate(10),
    move("F", "Y", 100),
    point("P", 5),
    move("B", "Y", 100),
    rotate(-10),

    # Panneau 3
    move("B", "X", 245),
    rotate(10),
    move("F", "Y", 100),
    point("P", 5),
    move("B", "Y", 100),
    #rotate(-10),  # viser les plantes
    # on rotate pas parce qu'on est dans la trajectoire déjà
    
    # Ouverture des pinces
    # dépend de la position de la pince au setup
    #claw("O","Z"),
    #gripper("U","Z",15), 
    gripper("D","Z",70),
    #claw("O","X"),
    gripper("D","X",70),
    claw("O","Y"),
    gripper("D","Y",70),

    # Plante 1
    move("F","Z",300),
    claw("C","Z"),
    move("B", "Z", 60),
    gripper("U", "Z", 100),
    rotate(-120),

    # Plante 2
    move("F", "Y", 120),
    claw("C", "Y"),
    move("B", "Y", 60),
    gripper("U", "Y", 100),
    rotate(-130),  # un peu plus pour viser les plantes restantes

    # Plante 3
    move("F", "X", 120),
    claw("C", "X"),
    move("B", "X", 60),
    gripper("U", "X", 100),
    rotate(-10),  # se tourner vers les pots
    
    # Vers les pots
    move("F", "Y", 655),  # problème avec la table!
    claw("O", "Y"),
    # ...
]  # STRATEGY_1B

STRATEGY_2B = []

STRATEGY_3B = []



STRATEGY_1J = []

STRATEGY_2J = []

STRATEGY_3J = []
