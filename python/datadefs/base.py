__author__ = 'matthias'


class Base(object):
    num = 1

    def print_info(self):
        print self.num

    def genString(self, argument , list=[]):
        string = []
        if len(list) != 0:
            for entry in list:
                string.append(self.branch + argument + entry)

        else:
            string = self.branch + argument

        return string