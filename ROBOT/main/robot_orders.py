def move(direction: str, axis: str, distance: int):
    """
    move: déplacer le robot sur l'aire de jeu

    direction: "F", "B"
    axis: "X", "Y", "Z"
    distance: entier
    """
    m = b"".join([direction.encode(), axis.encode(), str(distance).encode(), b"\n"])
    print("order:", m, sep=" -> ")
    return m


def stop():
    """
    stop: cesser le mouvement du robot sur l'aire de jeu
    """
    m = b"S\n"
    print("order:", m, sep=" -> ")
    return m

def pami(camp):
    """
    pami: Indiquer le camp vers lequel les pami iront
    """
    if camp =="g":
        m = b"g\n"
    if camp =="d":
        m = b"d\n"
    print("order:", m, sep=" -> ")
    return m

def restart():
    """
    To finish movement: Reprise d'un mouvement après un Stop
    """
    m = b"T\n"
    print("order:", m, sep=" -> ")
    return m


def rotate(angle: int):
    """
    rotate: effectuer une rotation de 'angle' degrés

    angle: entier entre -180 et 180
    """
    m = b"".join([b"R", str(angle).encode(), b"\n"])
    print("order:", m, sep=" -> ")
    return m


def gripper(direction: str, axis: str, distance: int):
    """
    gripper: monter et descendre les pinces

    direction: "U", "D"
    axis: "X", "Y", "Z"
    distance: entier
    """
    m = b"".join([direction.encode(), axis.encode(), str(distance).encode(), b"\n"])
    print("order:", m, sep=" -> ")
    return m

def point(point, val):
    print("Point")
    return point+str(val)


def claw(action: str, axis: str):
    """
    claw: ouvrir et fermer les pinces

    action: "O", "C"
    axis: "X", "Y", "Z"
    """
    m = b"".join([action.encode(), axis.encode(), b"\n"])
    print("order:", m, sep=" -> ")
    return m
