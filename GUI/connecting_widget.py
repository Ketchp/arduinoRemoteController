#!/usr/bin/env python3
from tkinter import *
from tkinter import ttk


class ConnectingPage:
    def __init__(self, parent, loop):
        self.loop = loop
        self.parent = parent
        self.root = ttk.Frame(parent, padding=15)
        self.text_conn = StringVar()
        self.label = ttk.Label(self.root, textvariable=self.text_conn)
        self.progressbar = ttk.Progressbar(self.root, orient=HORIZONTAL, length=200, mode='indeterminate')
        self.joke = StringVar()
        self.label_joke = ttk.Label(self.root, textvariable=self.joke)

        self.connecting = True

        self.label.grid(row=1, column=1)
        self.progressbar.grid(row=2, column=1)
        self.label_joke.grid(row=3, column=1)
        self.progressbar.start()

    def create_connection(self, address, port):
        import socket
        import threading
        self.connecting = True
        self.grid()
        threading.Thread(target=self.change_label, daemon=True).start()
        threading.Thread(target=self.change_joke, daemon=True).start()
        print('connecting')
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        try:
            s.connect((address, port))
        except OSError as e:
            print("No host")
            raise e

        print('connected')
        self.connecting = False
        self.loop(s, address, port)

    def grid(self):
        self.root.grid(row=0, column=0, sticky='wnes')
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        self.root.columnconfigure(2, weight=1)
        self.root.rowconfigure(4, weight=1)

    def destroy(self):
        self.root.destroy()

    def change_label(self):
        import time
        while self.connecting:
            for i in range(4):
                self.text_conn.set('Connecting'+'.'*i)
                time.sleep(1 if i != 4 else 2)
        print('Ending thread1')

    def change_joke(self):
        while self.connecting:
            pass
        print('Ending thread2')
            
