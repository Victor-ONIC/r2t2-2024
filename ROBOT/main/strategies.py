from robot_orders import *

STRATEGY1 = [
    
    pami("g"),

	claw("C", "Y"),
	gripper("D", "Z", 75),

    # Panneau 1
    move("B", "X", 50),
    rotate(11),
    move("F", "Y", 95),
    point("P", 5),
    move("B", "Y", 95),
    rotate(-11),
    
    # Panneau 2
    move("B", "X", 230),
    rotate(11),
    move("F", "Y", 82),
    point("P", 5),
    move("B", "Y", 82),
    rotate(-11),
    
    # Panneau 3
    move("B", "X", 230),
    rotate(11),
    move("F", "Y", 95),
    point("P", 5),
    move("B", "Y", 95),
    rotate(-6),  # vers les plantes

    claw("O", "Y"),
    gripper("D", "Y", 70),

	# Plante 1
    move("F", "Z", 305),
    claw("C", "Z"),
    move("B", "Z", 60),
    gripper("U", "Z", 100),
    rotate(-120),

	# Plante 2
    move("F", "Y", 120),
    claw("C", "Y"),
    move("B", "Y", 60),
    gripper("U", "Y", 100),
    rotate(-130),

	# Plante 3
    move("F", "X", 140),
    claw("C", "X"),
    move("B", "X", 28),
    gripper("U", "X", 100),
    rotate(50),  # TOF devant
    
    # Vers les pots
    move("B", "X", 530),
    rotate(-65),  # pot devant
    
    # Pot 1
    move("F", "Y", 100),
    claw("O", "Y"),
    move("B", "Y", 100),
    rotate(-40),  # tof devant
    move("B", "X", 50),
    rotate(-90),  # second pot devant, tourner plus
    
    # Pot 2
    move("F", "X", 100),
    claw("O", "X"),
    move("B", "X", 100),
    # Prendre le pot X ? il tombe tjrs... et ça fait des points PAMIs en plus en Zone 1
    rotate(-130),  # vers le pot arrière droit, en biais
    
    # Pot 3
    move("F", "Z", 170),
    claw("O", "Z"),
    move("B", "Z", 180),
    
    #Pousser les pots
    move("B", "X", 400),
    rotate(40),
    move("F", "X", 100),
    move("F", "X", 100),
    move("F", "X", 100),
    move("F", "X", 100),
    move("F", "X", 100),
    
    # Go zone de lancement
    move("B","X",1250),

]  # STRATEGY1

STRATEGY3 = [
    claw("C", "Y"),
    claw("O", "Y")
]

STRATEGY2 = [
    move("F", "Y", 1000),
    point("P", 15),
    rotate(120),
    point("P", 5),
    move("B", "Y", 1000),
    point("P", 20)
]

# Stratégie de secours, panneaux seulement
STRATEGYB4 = [
    #BLEU

	claw("C", "Y"),
	gripper("D", "Z", 75),

    # Panneau 1
    move("B", "X", 50),
    rotate(11),
    move("F", "Y", 95),
    point("P", 5),
    move("B", "Y", 95),
    rotate(-11),
    
    # Panneau 2
    move("B", "X", 225),
    rotate(11),
    move("F", "Y", 95),
    point("P", 5),
    move("B", "Y", 95),
    rotate(-11),
    
    # Panneau 3
    move("B", "X", 230),
    rotate(11),
    move("F", "Y", 95),
    point("P", 5),
    move("B", "Y", 95),
    rotate(-50),

    # Rentrer au bercail
    move("F","Z",1250),
    rotate(27),
    move("F","X",334),

]

# Stratégie de secours, panneaux seulement
STRATEGYJ4 = [
    #JAUNE

	claw("C", "Z"),
	gripper("D", "Y", 75),

    # Panneau 1
    move("B", "X", 50),
    rotate(-11),
    move("F", "Z", 95),
    point("P", 5),
    move("B", "Z", 95),
    rotate(11),
    
    # Panneau 2
    move("B", "X", 225),
    rotate(-11),
    move("F", "Z", 95),
    point("P", 5),
    move("B", "Z", 95),
    rotate(11),
    
    # Panneau 3
    move("B", "X", 230),
    rotate(-11),
    move("F", "Z", 95),
    point("P", 5),
    move("B", "Z", 95),
    rotate(50),

    # Rentrer au bercail
    move("F","Y",1514),
    rotate(-27),
    move("F","X",434),

]
