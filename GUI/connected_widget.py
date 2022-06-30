#!/usr/bin/env python3
from tkinter import ttk


class ConnectedPage:
    def __init__(self, parent):
        self.parent = parent
        self.frame = ttk.Frame(self.parent, padding=15)
        self.text1 = ttk.Label(self.frame, text='Connected')
        self.text2 = ttk.Label(self.frame, text='waiting for response...')
        self.text1.grid(row=1, column=1)
        self.text2.grid(row=2, column=1)
        self.display = False

    def grid(self):
        self.frame.grid(row=0, column=0, sticky='wnes')
        self.frame.columnconfigure(0, weight=1)
        self.frame.rowconfigure(0, weight=1)
        self.frame.columnconfigure(2, weight=1)
        self.frame.rowconfigure(3, weight=1)
        self.display = True

    def destroy(self):
        self.frame.grid_remove()
        self.frame.destroy()
        self.display = False
