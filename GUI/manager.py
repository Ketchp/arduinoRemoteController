#!/usr/bin/env python3
from widgets import *
from shared import SET_commands
from math import ceil


def parser(obj):
    if type(obj) in (tuple, list):
        return type(obj)(map(parser, obj))
    if type(obj) == dict:
        for key in list(obj):
            obj[key] = parser(obj[key])
    if type(obj) == str:
        return obj.replace('<deg>', '°')
    return obj


class Manager:
    def __init__(self, parent):
        self.master = parent
        self.mainframe = ttk.Frame(parent, padding=15)
        self.widgets = []
        self.template = False
        self.cols = 0
        self.rows = 0

    def new(self, template):
        self.template = True
        if type(template) == str:
            import json
            template = json.loads(template)
        template = parser(template)

        for row in range(self.rows):
            self.mainframe.rowconfigure(row, weight=0)
        for column in range(self.cols):
            self.mainframe.columnconfigure(column, weight=0)
        if 'geometry' in template:
            self.cols = 1
            self.rows = len(template['widgets'])
            if 'cols' in template['geometry']:
                self.cols = template['geometry']['cols']
                self.rows = ceil(len(template['widgets']) / self.cols)
                
            if 'rows' in template['geometry']:
                self.rows = template['geometry']['rows']
                self.cols = ceil(len(template['widgets']) / self.rows)
                
        for widget in self.widgets:
            widget.destroy()
        self.widgets = []
        for index, widget in enumerate(template['widgets']):
            widget_type_class = {'value': Value, 'V': Value,
                                 'enable': Enable, 'EN': Enable,
                                 'triple': Triple, 'T': Triple,
                                 'button': ButtonElement, 'B': ButtonElement,
                                 'triple_switch': TripleSwitch, 'TS': TripleSwitch,
                                 'entry': EntryElement, 'E': EntryElement,
                                 'switch': Switch, 'S': Switch,
                                 'label': LabelElement, 'L': LabelElement}
            try:
                widget_type = widget_type_class[widget['W']]
            except KeyError:
                print('Unknown widget')
                continue

            if 'position' not in widget:
                widget['position'] = [index // self.cols, index % self.cols]
                
            self.widgets.append(widget_type(widget, index, self.mainframe))
            
        for row in range(self.rows):
            self.mainframe.rowconfigure(row, weight=1)
        for column in range(self.cols):
            self.mainframe.columnconfigure(column, weight=1)
                    
    def update(self, update):
        if type(update) == str:
            import json
            update = json.loads(update)
        update = parser(update)
        if 'values' not in update:
            return
        for index, value in enumerate(update["values"]):
            self.widgets[index].update(value)

    def grid(self):
        self.mainframe.grid(row=0, column=0, sticky="NWES")
