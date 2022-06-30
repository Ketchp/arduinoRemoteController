#!/usr/bin/env python3
from tkinter import *
from tkinter import ttk
from shared import SET_commands


def unix_to_string(form):
    from datetime import datetime

    def func(unix_time):
        return datetime.utcfromtimestamp(unix_time).strftime(form)
    return func


def string_to_type(string, form="%d.%m. / %H:%M:%S"):
    if string in ('int', 'long', 'byte', 'I', 'L', 'B'):
        return int
    if string in ('float', 'double', 'F'):
        return float
    if string in ('bool', 'b'):
        return bool
    if string in ('datetime', 'DT'):
        return unix_to_string(form)
    return str


class Value:
    def __init__(self, obj, index, parent):
        self.name = obj['N']
        self.value = StringVar()
        if 'U' in obj:
            self.unit = obj['U']
        else:
            self.unit = ''
        self.type = string_to_type(obj['T'], self.unit)
        self.specialVal = {}
        if 'V' in obj:
            self.specialVal = obj['V']
            type_values = {}
            for key in self.specialVal:
                type_values[self.type(key)] = self.specialVal[key]
            self.specialVal.update(type_values)

        self.container = ttk.Frame(parent, padding=15, borderwidth=1, relief=SOLID)
        self.container.grid(column=obj['position'][1], row=obj['position'][0], padx=5, pady=5, sticky='wnes')
        self.Label_name = ttk.Label(self.container, text=self.name)
        self.Label_value = ttk.Label(self.container, textvariable=self.value)
        self.Label_name.grid(row=1, column=1)
        self.Label_value.grid(row=1, column=2)
        if obj['T'] not in ('datetime', 'DT'):
            self.Label_unit = ttk.Label(self.container, text=self.unit)
            self.Label_unit.grid(row=1, column=3)
        self.container.columnconfigure(0, weight=1)
        self.container.columnconfigure(4, weight=1)
        self.container.rowconfigure(0, weight=1)
        self.container.rowconfigure(2, weight=1)
        self.index = index

    def update(self, value):
        if value in self.specialVal:
            self.value.set(self.specialVal[value])
        else:
            self.value.set(self.type(value))

    def destroy(self):
        self.container.destroy()


class Enable:
    delimiter = ','

    def __init__(self, obj, index, parent):
        self.name = obj['N']
        self.value = StringVar()
        self.type = string_to_type(obj['T'])
        self.text = obj['text'].split(self.delimiter)
        self.container = ttk.Frame(parent, padding=15, borderwidth=1, relief=SOLID)
        self.container.grid(column=obj['position'][1], row=obj['position'][0], padx=5, pady=5, sticky='wnes')
        self.Label_name = ttk.Label(self.container, text=self.name)
        self.Label_value = ttk.Label(self.container, textvariable=self.value)
        self.Label_name.grid(row=1, column=1)
        self.Label_value.grid(row=1, column=2)
        self.container.columnconfigure(0, weight=1)
        self.container.columnconfigure(3, weight=1)
        self.container.rowconfigure(0, weight=1)
        self.container.rowconfigure(2, weight=1)
        self.index = index
        
    def update(self, value):
        self.value.set(self.text[self.type(value)])
        
    def destroy(self):
        self.container.destroy()


class Triple:
    delimiter = ','

    def __init__(self, obj, index, parent):
        self.name = obj['N']
        self.value = StringVar()
        self.type = string_to_type(obj['T'])
        self.text = obj['text'].split(self.delimiter)
        self.container = ttk.Frame(parent, padding=15, borderwidth=1, relief=SOLID)
        self.container.grid(column=obj['position'][1], row=obj['position'][0], padx=5, pady=5, sticky='wnes')
        self.Label_name = ttk.Label(self.container, text=self.name)
        self.Label_value = ttk.Label(self.container, textvariable=self.value)
        self.Label_name.grid(row=1, column=1)
        self.Label_value.grid(row=1, column=2)
        self.container.columnconfigure(0, weight=1)
        self.container.columnconfigure(3, weight=1)
        self.container.rowconfigure(0, weight=1)
        self.container.rowconfigure(2, weight=1)
        self.index = index

    def update(self, value):
        self.value.set(self.text[self.type(value)])
        
    def destroy(self):
        self.container.destroy()


class ButtonElement:
    def __init__(self, obj, index, parent):
        self.index = index
        self.name = obj['N']
        self.pressed = False
        self.enabled = True
        self.type = string_to_type(obj['T'])
        self.container = ttk.Frame(parent, padding=15)
        self.container.grid(column=obj['position'][1], row=obj['position'][0])
        self.style = ttk.Style()
        self.style_name = f'{self.index}custom.TButton'
        self.button = ttk.Button(self.container, text=self.name, command=self.callback,
                                 padding=(15, 10), style=self.style_name)
        self.button.grid(row=0, column=0)

    def update(self, value, *enabled):
        if enabled:
            enabled = enabled[0]
        else:
            value, enabled = value[0], value[1]
        self.pressed = self.type(value)
        self.enabled = bool(enabled)
        if self.pressed:
            self.button.state(['pressed'])
            self.style.configure(self.style_name, relief=SUNKEN)
        else:
            self.button.state(['!pressed'])
            self.style.configure(self.style_name, relief=RAISED)
        if self.enabled:
            self.button.state(['!disabled'])
        else:
            self.button.state(['disabled'])
        if self.button.focus_get() == self.button:
            self.container.focus()
        
    def callback(self):
        self.update(True, True)
        SET_commands.append({'CMD': 'SET', 'ID': self.index, 'value': self.type(self.pressed)})

    def destroy(self):
        self.container.destroy()


class TripleSwitch:
    delimiter = ','

    def __init__(self, obj, index, parent):
        self.name = obj['N']
        self.value = StringVar()
        self.enabled = True
        self.type = string_to_type(obj['T'])
        self.text = obj['text'].split(self.delimiter)
        self.container = ttk.Frame(parent, padding=15, borderwidth=1, relief=SOLID)
        self.container.grid(column=obj['position'][1], row=obj['position'][0], padx=5, pady=5, sticky='wnes')
        self.Label_name = ttk.Label(self.container, text=self.name)
        self.radio = [Radiobutton(self.container, text=self.text[i], variable=self.value, indicatoron=False,
                                  value=i, command=self.callback, padx=6, pady=4) for i in range(3)]
        self.Label_name.grid(row=1, column=1, columnspan=3)
        [self.radio[i].grid(row=2, column=i+1) for i in range(3)]
        self.container.columnconfigure(0, weight=1)
        self.container.columnconfigure(4, weight=1)
        self.container.rowconfigure(0, weight=1)
        self.container.rowconfigure(3, weight=1)
        self.index = index

    def update(self, value, *enabled):
        if enabled:
            enabled = enabled[0]
        else:
            value, enabled = value[0], value[1]

        self.enabled = enabled
        self.value.set(self.type(value))

        if self.enabled:
            temp = NORMAL
        else:
            temp = DISABLED

        [self.radio[i].config(state=temp) for i in range(3)]

    def callback(self):
        SET_commands.append({'CMD': 'SET', 'ID': self.index, 'value': self.type(self.value.get())})
    
    def destroy(self):
        self.container.destroy()


class EntryElement:
    def __init__(self, obj, index, parent):
        self.name = obj['N']
        self.value = StringVar()
        self.enabled = True
        self.type = string_to_type(obj['T'])
        if 'U' in obj:
            self.unit = obj['U']
        else:
            self.unit = ''

        self.specialVal = {}
        if 'V' in obj:
            self.specialVal = obj['V']
            type_values = {}
            for key in self.specialVal:
                type_values[self.type(key)] = self.specialVal[key]
            self.specialVal.update(type_values)

        self.container = ttk.Frame(parent, padding=15, borderwidth=1, relief=SOLID)
        self.container.grid(column=obj['position'][1], row=obj['position'][0], padx=5, pady=5, sticky='wnes')
        self.validation = self.container.register(self.check)
        self.Label_name = ttk.Label(self.container, text=self.name)
        self.show = None
        if 'show' in obj:
            if obj['show'] == 'password':
                self.show = '*'
        self.Entry = ttk.Entry(self.container, textvariable=self.value, justify='right', width=10,
                               validate='key', validatecommand=(self.validation, '%P'), show=self.show)
        self.Label_name.grid(row=1, column=1)
        self.Entry.grid(row=1, column=2)
        if obj['T'] not in ('datetime', 'DT'):
            self.Label_unit = ttk.Label(self.container, text=self.unit)
            self.Label_unit.grid(row=1, column=3)
        self.container.columnconfigure(0, weight=1)
        self.container.columnconfigure(4, weight=1)
        self.container.rowconfigure(0, weight=1)
        self.container.rowconfigure(2, weight=1)
        self.Entry.bind('<FocusOut>', self.lost_focus)
        self.Entry.bind('<Return>', self.return_press)
        self.Entry.bind('<FocusIn>', self.get_focus)
        self.index = index
        self.last = ''

    def get_focus(self, _):
        self.last = self.value.get()

    def return_press(self, _):
        self.container.focus()

    def lost_focus(self, _):
        try:
            if self.type(self.value.get()) != self.type(self.last):
                SET_commands.append({'CMD': 'SET', 'ID': self.index, 'value': self.type(self.value.get())})

        except ValueError:
            try:
                SET_commands.append({'CMD': 'SET', 'ID': self.index, 'value': self.type(self.value.get())})
            except ValueError:
                self.value.set(self.last)
            
    def update(self, value, *enabled):
        if enabled:
            enabled = enabled[0]
        else:
            value, enabled = value[0], value[1]

        if self.Entry != self.container.focus_get():
            self.enabled = enabled
            if value in self.specialVal:
                self.value.set(self.specialVal[self.type(value)])
            else:
                self.value.set(self.type(value))

    def check(self, text):
        if self.type == int:
            try:
                int(text)
                return True
            except ValueError:
                return text == ''

        if self.type == float:
            text = text.replace(',', '.')
            try:
                float(text)
                return True
            except ValueError:
                return text in ('', '.')
        return True

    def destroy(self):
        self.container.destroy()
    

class Switch:
    def __init__(self, obj, index, parent):
        self.index = index
        self.name = obj['N']
        self.pressed = False
        self.enabled = True
        self.type = string_to_type(obj['T'])
        self.container = ttk.Frame(parent, padding=15)
        self.container.grid(column=obj['position'][1], row=obj['position'][0])
        self.style = ttk.Style()
        self.style_name = f'{self.index}custom.TButton'
        self.button = ttk.Button(self.container, text=self.name, command=self.callback,
                                 padding=(15, 10), style=self.style_name)
        self.button.grid(row=0, column=0)

    def update(self, value, *enabled):
        if enabled:
            enabled = enabled[0]
        else:
            value, enabled = value[0], value[1]
        self.pressed = self.type(value)
        self.enabled = bool(enabled)
        if self.pressed:
            self.button.state(['pressed'])
            self.style.configure(self.style_name, relief=SUNKEN)
        else:
            self.button.state(['!pressed'])
            self.style.configure(self.style_name, relief=RAISED)
        if self.enabled:
            self.button.state(['!disabled'])
        else:
            self.button.state(['disabled'])
        if self.button.focus_get() == self.button:
            self.container.focus()

    def callback(self):
        self.update(not self.pressed, True)
        SET_commands.append({'CMD': 'SET', 'ID': self.index, 'value': self.type(self.pressed)})

    def destroy(self):
        self.container.destroy()


class LabelElement:
    def __init__(self, obj, index, parent):
        self.name = obj['N']
        self.value = StringVar()
        self.container = ttk.Frame(parent, padding=15, borderwidth=1, relief=SOLID)
        self.container.grid(column=obj['position'][1], row=obj['position'][0], padx=5, pady=5, sticky='wnes')
        self.Label_name = ttk.Label(self.container, text=self.name)
        self.Label_name.grid(row=1, column=1)
        self.container.columnconfigure(0, weight=1)
        self.container.columnconfigure(2, weight=1)
        self.container.rowconfigure(0, weight=1)
        self.container.rowconfigure(2, weight=1)
        self.index = index

    def update(self, value):
        self.value.set(value)

    def destroy(self):
        self.container.destroy()
