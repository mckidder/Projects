
# Simple and inefficient script to compare output to verify
# bits generated are indeed random (looking for duplication
# on a source file of over 10k time stamps that were converted)

f = open('p2test.txt', 'r')
f_match = open('p3test.txt', 'r')
f3 = open('compare.txt', 'a+')
#print f
dictionary = []
for line in f:
    a = line.split()
    dictionary.append(a)

    for f_match in dictionary:
	   # Following lines only used for verification of script function
       # f3.write("Compared ")
       # f3.write(line.split()[0])
       # f3.write(" with ")
       # f3.write(str(dictionary))
       # f3.write("\n")
       
        if line.split()[0] in dictionary:
            f3.write("Found duplicate ")
            f3.write(line.split()[0])
            f3.write(" \n")
