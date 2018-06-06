
class MyClass(object):
	def __init__(self, name):
		self.name = name#   w w w .j  a va  2 s  . co  m

	def __str__(self):
		print("id:","%x"%id(self))
		rep = "MyClass object\n"
		rep += "name: " + self.name + "\n"
		return rep

	def __cmp__(self, other):
		if self.name > other.name:
			return 1
		if self.name < other.name:
			return -1
		if self.name == other.name:
			return 0      

	def talk(self):
		print("Hi.  I'm", self.name, "\n")

crit1 = MyClass("A")
#crit1.talk()

crit2 = MyClass("B")
#crit2.talk()

print(crit1)

#print(crit1.name)
