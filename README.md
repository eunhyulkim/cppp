# C+++ <img src="https://badgen.net/badge/c+++/1.2.1/CF566F" width="125" height="24">
<img src="/img/cppp_logo.png" align="right"
     alt="Size Limit logo by eunhkim" width="120" height="178">

**c+++** is a powerful tool to help your **c++ project**. 
It was developed based on the clang++ compiler of mac os(10.15.6), 
and compiled with -std=c++17. Currently, it supports the following seven key features.

* Create a c++ file with **canonical templates**.
* Set **inheritance** relationship.
* Set the **getter** functions.
* Update the **member functions** of the header file to the source file.
* Declare an **exception** into an inner class of object class.
* Create **main** header and source file.
* Create **Makefile**.

<p align="center">
  <img src="./img/terminal.png" alt="Size Limit CLI" width="738">
</p>


# Efficiency
|                            | 👌 Support  |
| -------------------------- | :----------------: |
| Create file (.cpp, .hpp)   |         ✔️         |
| class template (.cpp)      |         ✔️         |
| Canonical form (.cpp, .hpp)|         ✔️         |
| Include base class         |         ✔️         |
| Set inheritance relationship |         ✔️         |
| Virtual function overload  |         ✔️         |
| Getter function (.cpp, .hpp)  |         ✔️         |
| Exception canonical form   |         ✔️         |
| Create Main template       |         ✔️         |
| Create Makefile            |         ✔️         |

With just a few typing, it will save a lot of your time!

# Usage
<p align="center">
  <img src="./img/demo.gif" alt="Size Limit CLI" width="738">
</p>

    git clone https://github.com/eunhyulkim/cppp.git cppp & cd cppp;
    clang++ -std=c++17 *.cpp -o c+++;
    cp c+++ ~/.c+++;
    echo "alias c+++='~/.c+++';' >> ~/.zshrc;
    source ~/.zshrc;
    c+++ -help;
If you are a Mac OS user, it would be convenient to **use the compiled program file** right away.
  
# IMPLEMENTATION NOTES
C is procedural-oriented and simple. But C++ is object-oriented and a little
more complex. What does this difference mean? In C, if you have simply written
a story in your head, in C++, you must first create the world and its characters in
which the story will be made. Just as you don't exist, the narrative is carried out by
the interaction between the character and the world. Yeah. That's what the object
means. Hiding you as a developer in the interaction of sophisticated objects.

You have to be responsible for many things, including the state, function, creation,
copy, and destruction of objects, but once you've designed the objects well, a lot
will be easier. The problem is that the design of the object is extremely troublesome,
complex and cumbersome. omg... Fortunately, if you understand the grammar of C++
and deal with this tool, it will give you wings on your back without taking away
the strength of your legs.

# WARNING
Object design is first. Do programming at the end. When all objects are designed, 
turn on C+++ and execute the commands in the order of start, inherit, getter, exit, 
function, main, and make.Otherwise, you will encounter unexpected results.

If possible, do a backup before using this tool. Otherwise, run it with the code file open. 
If used correctly, there will be no problems, but mistakes can lead to accidents that are difficult to reverse.


# BUG OR IDEA
If you're a 42 cadet or vocal in BonetoBeCode Slack, give dm to **eunhkim**. If not, please email me at valhalla.host@gmail.com.
