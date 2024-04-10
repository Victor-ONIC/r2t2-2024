#! /usr/bin/env python3
#  -*- coding: utf-8 -*-
#
# Support module generated by PAGE version 8.0
#  in conjunction with Tcl version 8.6
#    Mar 30, 2024 01:20:29 AM CET  platform: Windows NT

import sys
import tkinter as tk
import tkinter.ttk as ttk
from tkinter.constants import *
import os.path

import app_run

import time

import CCommunication2 as com
from strategies import *
from threading import Timer, Thread, Event

# Périphérique de communication série
# PORT = "/dev/ttyUSB0"
#PORT = "/dev/ttyACM0"
PORT = "COM11"
BAUDRATE = 9600
score = 0

com = com.communication(PORT)

_location = os.path.dirname(__file__)
_debug = True # False to eliminate debug printing from callback functions.

def main(*args):
    '''Main entry point for the application.'''
    global root
    root = tk.Tk()
    root.protocol( 'WM_DELETE_WINDOW' , root.destroy)
    # Creates a toplevel widget.
    global _top1, _w1
    _top1 = root
    _w1 = app_run.Toplevel1(_top1)
    root.mainloop()

def bleu(*args):
    if _debug:
        print('app_run_support.bleu')
        for arg in args:
            print ('    another arg:', arg)
        sys.stdout.flush()
    _w1.top.configure(background="#005B8C")
    _w1.bt_jaune.destroy()
    _w1.bt_bleu.configure(relief=FLAT)
    _w1.Label0.destroy()
    photo_location = os.path.join(_location,"./quit_bleu.png")
    global _img1
    _img1 = tk.PhotoImage(file=photo_location)
    _w1.bt_quitt.configure(image=_img1)
    affichage_r2t2("bleu")
    affichage_farming_mars("bleu")
    connexion()
    print("Stratégie 1")
    com.thread_strategy(STRATEGY1)


def jaune(*args):
    if _debug:
        print('app_run_support.vert')
        for arg in args:
            print ('    another arg:', arg)
        sys.stdout.flush()
    _w1.top.configure(background="#FFBF00")
    _w1.bt_bleu.destroy()
    _w1.bt_jaune.configure(relief=FLAT)
    _w1.Label0.destroy()
    photo_location = os.path.join(_location,"./quit_jaune.png")
    global _img1
    _img1 = tk.PhotoImage(file=photo_location)
    _w1.bt_quitt.configure(image=_img1)
    affichage_r2t2("jaune")
    affichage_farming_mars("jaune")
    connexion()
    print("Stratégie 2")
    com.thread_strategy(STRATEGY2)

def affichage_r2t2(couleur):
    _w1.logo_r2t2 = tk.Label(_w1.top)
    _w1.logo_r2t2.place(x=440, y=50, height=200, width=200)
    global _img0, _location
    photo_location = os.path.join(_location,"./logo_r2t2_"+couleur+".png")
    _img0 = tk.PhotoImage(file=photo_location)
    _w1.logo_r2t2.configure(image=_img0)

def affichage_farming_mars(couleur):
    _w1.logo_farming_mars = tk.Label(_w1.top)
    _w1.logo_farming_mars.place(x=80, y=50, height=200, width=200)
    global _img2, _location
    photo_location = os.path.join(_location,"./farming_mars_"+couleur+".png")
    _img2 = tk.PhotoImage(file=photo_location)
    _w1.logo_farming_mars.configure(image=_img2)

def connexion(*args):
    if _debug:
        print('test2_support.connexion')
        for arg in args:
            print ('    another arg:', arg)
        sys.stdout.flush()

    try:
        com.connexion()
        done()
        return True
    except Exception as e:
        # Just print(e) is cleaner and more likely what you want,
        # but if you insist on printing message specifically whenever possible...
        print(e)
        return False

def deconnexion(*args):
    if _debug:
        print('test2_support.deconnexion')
        for arg in args:
            print ('    another arg:', arg)
        sys.stdout.flush()
    com.deconnexion()

def done():
    global score
    if com.r is not None and com.r[0]=="P":
        score += int(com.r[1:])
        _w1.Label1.configure(text=str(score))
        com.r = None
    _w1.t = Timer(0.01, done)
    _w1.t.start()

if __name__ == '__main__':
    app_run.start_up()




