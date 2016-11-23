#!/usr/bin/python
from random import randint      #used to generate a random integer


#Create empty string
myNumbers = ''

#opening the 3 files and naming them file(i).txt as write only
myFiles = [open('file'+str(i)+'.txt', 'w') for i in range(3)]
for i in range(3):
   for j in range(10):                      #storing 10 rand characters
      myNumbers = myNumbers+(chr(randint(97, 122)))
   myNumbers=myNumbers+'\n'                 #append a newline char last
   myFiles[i].write(myNumbers)              #write string to file
   myNumbers=''                             #reset string contents

#close the file
for i in range(3):
   myFiles[i].close()
del myFiles[:]

#re-open files as read only to print out contents of files
myFiles = [open('file'+str(i)+'.txt', 'r') for i in range(3)]
for i in range(3):
   print "Contents of ", myFiles[i].name + ": ", myFiles[i].read()

#close the file, again
for i in range(3):
   myFiles[i].close()

#Generate 2 random ints
randInt1=randint(1, 42)
randInt2=randint(1, 42)

#printing out 2 random integers, and then product
print "Random Int 1: ", randInt1
print "Random Int 2: ", randInt2
print "Product of Int 1, and Int 2: ", randInt1*randInt2
