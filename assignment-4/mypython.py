#!/usr/bin/python
from random import randint      #used to generate a random integer

myNumbers = ''

#opening the 3 files and naming them file(i).txt
myFiles = [open('file'+str(i)+'.txt', 'w') for i in range(3)]
for x in range(3):
   for y in range(10):                      #storing 10 rand characters
      myNumbers = myNumbers+(chr(randint(97, 122)))
   myNumbers=myNumbers+'\n'                 #append a newline char last
   myFiles[x].write(myNumbers)              #write string to file
   myNumbers=''                             #reset string contents

#close the file
for j in range(3):
   myFiles[j].close()
del myFiles[:]

#re-open files as read only
myFiles = [open('file'+str(i)+'.txt', 'r') for i in range(3)]
for i in range(3):
   print "Contents of ", myFiles[i].name + ": ", myFiles[i].read()
#myString = myFiles[0].read()
#print "contents of string: ", myString

#close the file, again
for j in range(3):
   myFiles[j].close()
