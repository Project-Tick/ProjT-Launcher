## JavaCheck `javacheck/`

A very simple program to print provided system properties.

Each argument is printed out as `{arg}={System.getProperty(arg)}`. If any properties are null,
they will not be printed and the program will exit with code 1.
