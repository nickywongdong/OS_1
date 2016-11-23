#!/usr/bin/python
from random import randint      #used to generate a random integer



myNumbers = ''
#for i in range(10)
#   myNumbers.append(chr(randint(97, 122)))  #97-122, lowercase ascii's

#opening the 3 files and naming them file(i).txt
myFiles = [open('file'+str(i)+'.txt', 'wb+') for i in range(3)]
for x in range(3):
   for y in range(10):                          #storing 10 rand characters
      #if y==10:   break
      #print "y: ", y
      myNumbers = myNumbers+(chr(randint(97, 122)))
   print myNumbers[:]
   myNumbers=myNumbers+'\n'                 #append a newline char last
   myFiles[x].write(myNumbers)               #write contents of list to file
   myNumbers=''                             #clear contents of the list
   #rint "i'm outside of y loop\n"

#testing
for i in myFiles:
   print "Closed or not: ", i.closed
   print "File has: ", i.read()
#print "file: ", myFiles[0].name, myFiles[0].mode
str = myFiles[0].read()
print "file has: ", str
#fo_1=open("file1.txt" , "wb+")
#print "Name of the file: ", fo_1.name
#print "closed or not : ", fo_1.closed
#print "Opening mode : ", fo_1.mode
#print "Softspace flag : ", fo_1.softspace

#close the file
#fo_1.close()
for j in range(3):
   myFiles[j].close()
