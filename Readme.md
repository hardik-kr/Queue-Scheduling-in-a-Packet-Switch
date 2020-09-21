****************************************Assingment 2 Readme***************************************

A) By mannually compiling the program

        1) Compile the program using "g++ Q1.cpp" then executable file name will be "a.out"
            or
           Compile the program using "g++ -o ./routing Q1.cpp" then executable file wil be "routing"
        2) Now execute the executable file by passing the 7 argument in the command line 

            eg :-  ./routing 8 4 0.5 INQ 4 Output.txt 10000

            After that 1 files will be created as follows :-
            - Output.txt

            and also final output will be displayed into the terminal.

B) Run using the Makefile (Automatic compile and execute)

        1) Run the command in the terminal "make -f Makefile"
           It will automatically compile and execute the program with the given argument defined in the makefile, if you want to change the argument change in the makefile and again run the makefile

        2) Final Output will be displayed and 1 Files will be created.

    Note :- Default values are already placed in makefile to change the values change the input parameter in the makefile itself or use mannual compile and  execute
            While giving the input parameter queue (i.e scheduling) use all CAPS i.e (INQ | KOUQ | ISLIP )  
            Order of input parameter must be same as that of given in assignment.