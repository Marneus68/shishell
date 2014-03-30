shishell
========

Shishell is an attempt at writing an interactive shell with a vi-like input mode that supports yanking and registers.   

Features
--------

Shishell is still in active devellopement. Don't exect it to be your average full featured interactive shell. It still lacks a lot of essential features like backtick expressions, pipes and all that, but all these will be added as I figure how to implement these.  

Compilation and Installation
----------------------------

Shishell only relies on the C standard library and uses the POSIX terminal interface. You can build the project by running:
    
    make

and install it by running:

    make install

with *root privileges*.

Once installed, shishell can be run by running:

    shish

If you feel adventurous, you can set shishell as your default shell by running the following command:

    chsh -s /usr/bin/shish <username>

Usage
-----

Shishell can be invoked with several command line parameters :
    
    -i or --input       : Starts Shishell in INPUT mode (default behaviour
    -n or --normal      : Starts Shishell in NORMAL mode
    -v or --verbose     : Makes Shishell's startup more verbose
    -h or --help        : Prints the usage and help lines and exits
    -V or --version     : Prints the program name and version and exits

License
-------

This project is under the GNU Lesser General Public License version 2.1. See [LICENSE.txt](LICENSE) for more informations.  
