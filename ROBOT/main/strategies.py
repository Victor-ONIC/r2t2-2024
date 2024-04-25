from robot_orders import *

STRATEGY_1 = [
    claw("C", "Y"),
    
    # Panneau 1
    move("B", "X", 50),
    rotate(10),
    move("F", "Y", 105),
    point("P", 5),
    move("B", "Y", 105),
    rotate(-10),

    # Panneau 2
    move("B", "X", 235),
    rotate(10),
    move("F", "Y", 105),
    point("P", 5),
    move("B", "Y", 105),
    rotate(-10),

    # Panneau 3
    move("B", "X", 235),
    rotate(10),
    move("F", "Y", 100),
    point("P", 5),
    move("B", "Y", 100),
    rotate(-15),
    
    # Ouverture des pinces
    claw("O","Z"),
    gripper("U","Z",15),
    claw("O","X"),
    gripper("D","X",60),
    claw("O","Y"),
    gripper("D","Y",60),

    # Plante 1
    move("F","Z",300),
    claw("C","Z"),
    move("B", "Z", 60),
    gripper("U", "Z", 100),
    rotate(-120),

    # Plante 2
    move("F", "Y", 110),
    claw("C", "Y"),
    move("B", "Y", 60),
    gripper("U", "Y", 100),
    rotate(-120),

    # Plante 3
    move("F", "X", 120),
    claw("C", "X"),
    move("B", "X", 60),
    gripper("U", "X", 100),
    rotate(-140),
    
    # Vers les pots
    move("F", "X", 790),  # problème!
    claw("O", "X"),
]

STRATEGY_2 = [
    move("F", "X", 300),
]
