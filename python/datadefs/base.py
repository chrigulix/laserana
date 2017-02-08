__author__ = 'matthias'
import root_numpy as rn

class Base(object):
    def __init__(self):
        self.num = 1
        self.data_product = None
        self.tree = None
        self.branch = None
        self.filename = None

    def info(self):
        print self.num

    def gen_string(self, argument, arguments=None):
        if arguments is None:
            arguments = []
        string = []
        if len(arguments) != 0:
            for entry in arguments:
                string.append(self.branch + argument + entry)
        else:
            string = self.branch + argument

        return string

    def set_in_file(self, filename, path):
        self.filename = str(path) + str(filename)

    def get_tree(self):
        if self.tree is None:
            raise ValueError("no tree defined yet")
        return self.tree