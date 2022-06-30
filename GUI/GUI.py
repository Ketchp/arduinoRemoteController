#!/usr/bin/python3

from tkinter import *
from login_widget import LogPage
from connecting_widget import ConnectingPage
from connected_widget import ConnectedPage
from manager import Manager
from shared import SET_commands
import threading
import communication

core = Tk()
core.title('Controller')
core.columnconfigure(0, weight=1)
core.rowconfigure(0, weight=1)


def loop(sock, address, port):
    global conn_page, SET_commands
    conn_page.destroy()
    conn_page = ConnectedPage(core)
    conn_page.grid()
    manager = Manager(core)
    
    threading.Thread(target=communication.comm,
                     args=(sock, manager, conn_page, core, address, port),
                     daemon=True).start()

 
conn_page = ConnectingPage(core, loop)

try:
    log_page = LogPage(core, conn_page.create_connection, open('socket.dat', 'rb'))
except FileNotFoundError:
    from pickle import dump
    dump(('...', 0), open('socket.dat', 'wb'))
    log_page = LogPage(core, conn_page.create_connection, open('socket.dat', 'rb'))

log_page.grid()


def on_closing():
    global SET_commands
    SET_commands.append({'CMD': 'QUIT'})


core.protocol("WM_DELETE_WINDOW", on_closing)
core.mainloop()
