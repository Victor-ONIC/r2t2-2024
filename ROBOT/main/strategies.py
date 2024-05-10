from robot_orders import *

STRATEGY1 = [
    
    pami("d"),

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



















# 3 panneaux 1 plante et pamis: BLEU
STRATEGY2B = [

	pami("d"),
	
	claw("C", "Y"),
	gripper("D", "Z", 70),

    # Panneau 1
    move("B", "X", 50),
    rotate(11),
    move("F", "Y", 88),
    point("P", 5),
    move("B", "Y", 88),
    rotate(-11),
    
    # Panneau 2
    move("B", "X", 225),
    rotate(11),
    move("F", "Y", 100),
    point("P", 5),
    move("B", "Y", 100),
    rotate(-11),
    
    # Panneau 3
    move("B", "X", 230),
    rotate(11),
    move("F", "Y", 102),
    point("P", 5),
    move("B", "Y", 102),
	rotate(-6),  # vers les plantes

	# Plante 1
#    move("F", "Z", 150),
#    move("F", "Z", 155),
	move("F", "Z", 380),
    claw("C", "Z"),
    move("B", "Z", 60),
    gripper("U", "Z", 5),
    rotate(-158),  # vers zone 3

	# Déposer la plante dans la zone 3
	move("F", "Z", 500),
	gripper("D", "Z", 5),
	claw("O", "Z"),
	move("B", "Z", 66),
	
	# Vers la zone 1
#	rotate(125),
#	move("F", "Z", 200),
#	move("F", "Z", 200),
#	move("F", "Z", 200),
#	move("F", "Z", 200),
#	move("F", "Z", 200),
#	move("F", "Z", 200),
#	move("F", "Z", 200),
#	move("F", "Z", 200),  # toucher le mur
#	move("B", "Z", 250),
#	move("B", "Z", 250),
	rotate(-125),  # X
	move("F", "X", 1100),
	move("B", "Z", 222),  # assurer la rentrée en zone 1
	claw("O", "X"),
	gripper("U", "X", 90),
	
	point("P", 25),

]  # STRATEGY2B





















# 3 panneaux 1 plante et pamis: JAUNE
STRATEGY2J = [
	#JAUNE
	pami("g"),
	claw("C", "Z"),
	gripper("D", "Y", 70),

    # Panneau 1
    move("B", "X", 50),
    rotate(-11),
    move("F", "Z", 88),
    point("P", 5),
    move("B", "Z", 88),
    rotate(11),
    
    # Panneau 2
    move("B", "X", 225),
    rotate(-11),
    move("F", "Z", 100),
    point("P", 5),
    move("B", "Z", 100),
    rotate(11),
    
    # Panneau 3
    move("B", "X", 230),
    rotate(-11),
    move("F", "Z", 102),
    point("P", 5),
    move("B", "Z", 102),
    rotate(6),  # Y vers les plantes
	
	# Plante 1
    move("F", "Y", 380),
    claw("C", "Y"),
    move("B", "Y", 60),
    gripper("U", "Y", 5),
    rotate(158),  # vers zone 3

	# Déposer la plante dans la zone 3
#	move("F", "Z", 210),
#	move("F", "Z", 210),
	move("F", "Y", 500),
	gripper("D", "Y", 5),
	claw("O", "Y"),
	move("B", "Y", 66),
	
	# Vers la zone 1
	rotate(125),  # X
	move("F", "X", 1100),
	move("B", "Y", 222),  # assurer la rentrée en zone 1
	claw("O", "X"),
	gripper("U", "X", 90),
	
	point("P", 25),

]  # STRATEGY2J












STRATEGY3B = [

	pami("d"),
	
	claw("C", "Y"),
	gripper("D", "Z", 70),

    # Panneau 1
    move("B", "X", 50),
    rotate(11),
    move("F", "Y", 88),
    point("P", 5),
    move("B", "Y", 88),
    rotate(-11),
    
    # Panneau 2
    move("B", "X", 225),
    rotate(11),
    move("F", "Y", 100),
    point("P", 5),
    move("B", "Y", 100),
    rotate(-11),
    
    # Panneau 3
    move("B", "X", 230),
    rotate(11),
    move("F", "Y", 102),
    point("P", 5),
    move("B", "Y", 102),
	rotate(-6),  # vers les plantes

	# Plante 1
#    move("F", "Z", 150),
#    move("F", "Z", 155),
	move("F", "Z", 380),
    claw("C", "Z"),
    move("B", "Z", 60),
    gripper("U", "Z", 5),
    rotate(140),	# X prêt à attraper plante 2
    
    # Plante 2
#    claw("O", "X"),
    move("F", "X", 120),
    claw("C", "X"),
    move("B", "X", 120),
    rotate(-140),
    rotate(-158),  # Z vers zone 3

	# Déposer la plante dans la zone 3
	move("F", "Z", 500),
	gripper("D", "Z", 5),
	claw("O", "Z"),
	move("B", "Z", 66),
	
	# Vers la zone 1
	rotate(-125),  # X
	move("F", "X", 1000),
	move("B", "Z", 256),  # assurer la rentrée en zone 1
	
	# poser plante en zone 1
	move("F", "X", 300),
	claw("O", "X"),
	move("B", "X", 100),
	gripper("U", "X", 90),
	
	point("P", 30),

]  # STRATEGY3B









### TODO ####################################################
STRATEGY3J = [
	#JAUNE
	pami("g"),
	claw("C", "Z"),
	gripper("D", "Y", 70),

    # Panneau 1
    move("B", "X", 50),
    rotate(-11),
    move("F", "Z", 88),
    point("P", 5),
    move("B", "Z", 88),
    rotate(11),
    
    # Panneau 2
    move("B", "X", 225),
    rotate(-11),
    move("F", "Z", 100),
    point("P", 5),
    move("B", "Z", 100),
    rotate(11),
    
    # Panneau 3
    move("B", "X", 230),
    rotate(-11),
    move("F", "Z", 102),
    point("P", 5),
    move("B", "Z", 102),
    rotate(6),  # Y vers les plantes
	
	# Plante 1
    move("F", "Y", 380),
    claw("C", "Y"),
    move("B", "Y", 60),
    gripper("U", "Y", 5),
    rotate(120),	# Z prêt à attraper plante 2
    
    # Plante 2
    claw("O", "Z"),
    gripper("D", "Z", 70),
    move("F", "Z", 100),
    claw("C", "Z"),
    move("B", "Z", 100),
    gripper("U", "Z", 5),
    rotate(38),  # Y vers zone 3

	# Déposer la plante dans la zone 3
#	move("F", "Z", 210),
#	move("F", "Z", 210),
	move("F", "Y", 480),
	gripper("D", "Y", 5),
	claw("O", "Y"),
	move("B", "Y", 66),
	
	# Vers la zone 1
	rotate(125),  # X
	move("F", "X", 1000),
	move("B", "Y", 322),  # assurer la rentrée en zone 1
	
	# poser plante en zone 1
	move("F", "X", 300),
	claw("O", "X"),
	move("B", "X", 100),
	gripper("U", "X", 90),
	
	point("P", 25),

]  # STRATEGY3J


# Stratégie de secours, panneaux seulement: BLEU
STRATEGY4B = [
    #BLEU
	pami("d"),
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
#    point("P", 5),
    move("B", "Y", 95),
    rotate(-50),

    # Rentrer au bercail
#    move("F", "Z", 400),
#    move("F", "Z", 400),
#    move("F", "Z", 500),
    move("F", "Z", 250),
    move("F", "Z", 250),
    move("F", "Z", 250),
    move("F", "Z", 250),
    move("F", "Z", 250),
	
	point("P", 10),
	point("P", 5),
	point("P", 5),
]

# Stratégie de secours, panneaux seulement: JAUNE
STRATEGY4J = [
    #JAUNE
	pami("g"),
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
#    point("P", 5),
    move("B", "Z", 95),
    rotate(150),

    # Rentrer au bercail
#    move("F", "Z", 400),
#    move("F", "Z", 400),
#    move("F", "Z", 500),
    move("F", "Z", 250),
    move("F", "Z", 250),
    move("F", "Z", 250),
    move("F", "Z", 250),
    move("F", "Z", 250),

	point("P", 10),
	point("P", 5),
	point("P", 5),
]
