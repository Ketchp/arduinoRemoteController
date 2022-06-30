#!/usr/bin/env python3
from tkinter import *
from tkinter import ttk, messagebox
import threading


class LogPage:
    def __init__(self, parent, start_func, file):
        from pickle import load
        self.start_func = start_func
        self.root = ttk.Frame(parent, padding=15)
        self.address = '0.0.0.0'
        self.port = 0
        self.ip_var = [StringVar() for _ in range(4)]
        self.port_var = StringVar()
        address, port = load(file)
        [self.ip_var[index].set(ip) for index, ip in enumerate(address.split('.'))]
        self.port_var.set(port)
        self.IP_valid = self.root.register(self.ip_validation)
        self.port_valid = self.root.register(self.port_validation)
        self.label = ttk.Label(self.root, text='Enter IP address and port:')
        self.entries = [ttk.Entry(self.root, textvariable=self.ip_var[i], justify='right', width=3,
                                  validate='key', validatecommand=(self.IP_valid, '%P')) for i in range(4)]
        self.entry_port = ttk.Entry(self.root, textvariable=self.port_var, width=5, validate='key',
                                    validatecommand=(self.port_valid, '%P'))
        self.button = ttk.Button(self.root, text='Connect', command=self.connect)
        self.separators = [ttk.Label(self.root, text='.') for _ in range(3)]
        self.separator_port = ttk.Label(self.root, text=':')
        self.label.grid(row=1, column=1, columnspan=9, sticky=S, pady=(0, 6))
        [self.entries[i].grid(row=2, column=2*i+1) for i in range(4)]
        self.entry_port.grid(row=2, column=9)
        [self.separators[i].grid(row=2, column=2*i + 2) for i in range(3)]
        self.separator_port.grid(row=2, column=8)
        self.button.grid(row=3, column=1, columnspan=9, pady=(7, 0), sticky=N)
        self.entries[0].focus()
        self.entries[0].icursor(3)
        self.button.bind('<Return>', self.connect)
        self.entry_port.bind('<Return>', self.connect)

    @staticmethod
    def ip_validation(text):
        try:
            if text or 0 <= int(text) < 256:
                if text and text[0] == '0' and len(text) > 1:
                    return False
                return True
        except ValueError:
            return False

    @staticmethod
    def port_validation(port):
        try:
            if port or 0 <= int(port) < 65536:
                if port and port[0] == '0' and len(port) > 1:
                    return False
                return True
        except ValueError:
            return False

    def connect(self):
        if '' in map(StringVar.get, self.ip_var):
            for i in range(4):
                if self.ip_var[i].get() == '':
                    self.entries[i].focus()
                    break
            messagebox.showwarning(message='Missing IP address', title='Missing information')
            return
        if '' == self.port_var.get():
            self.entry_port.focus()
            messagebox.showwarning(message='Missing port number', title='Missing information')
            return
        self.address = '.'.join(map(StringVar.get, self.ip_var))
        self.port = int(self.port_var.get())
        from pickle import dump
        dump((self.address, self.port), open('socket.dat', 'wb'))
        self.destroy()
        threading.Thread(target=self.start_func, args=(self.address, self.port), daemon=True).start()
        print("returning")
        
    def grid(self):
        self.root.grid(row=0, column=0, sticky="wnes")
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        self.root.columnconfigure(10, weight=1)
        self.root.rowconfigure(4, weight=1)
        
    def destroy(self):
        self.root.destroy()
