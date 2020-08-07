#include "cppp.hpp"
#include <stdio.h>

namespace {
	void print(void)
	{
		/* HEADER */
		printf("%-30s%30s%30s\n\n", "C+++", "\033[5;30;47m  CPPP Library Functions Manual  \033[0m", "C+++");

		/* HELP */
		printf("\033[1mHELP\033[0m\n");
		printf("%5s-h, -H, -help, -HELP, -Help\n\n", " ");

		/* CHEAT SHEET */
		printf("\033[1mCHEAT SHEET\033[0m\n");
		printf("%5s\033[1mStart\033[0m : -s, -S, -start, -START, -Start\n", " ");
		printf("%5s\033[1mStart+\033[0m : -s+, -S+, -start+, -START+, -Start+\n", " ");
		printf("%5s\033[1mInherit\033[0m : -i, -I, -inherit, -INHERIT, -Inherit\n", " ");
		printf("%5s\033[1mStrong Getter\033[0m : -g, -G, -getter, -GETTER, -Getter\n", " ");
		printf("%5s\033[1mFunction\033[0m : -f, -F, -function, -FUNCTION, -Function\n", " ");
		printf("%5s\033[1mException\033[0m : -e, -E, -exception, -EXCEPTION, -Exception\n", " ");
		printf("%5s\033[1mMain\033[0m : -m, -M, -main, -MAIN, -Main\n", " ");
		printf("%5s\033[1mMakefile\033[0m : -m+, -M+, -make, -MAKE, -Make\n\n", " ");

		/* SYNOPSIS */
		printf("\033[1mSYNOPSIS\033[0m\n");
		printf("%5s\033[1mc+++ -start\033[0m \033[4mfile_name_to_create\033[0m [\033[4m...\033[0m]\n", " ");
		printf("%5s\033[1mc+++ -start+\033[0m \033[4mfile_name_to_create\033[0m [\033[4m...\033[0m]\n", " ");
		printf("%5s\033[1mc+++ -inherit\033[0m \033[4mbase_class\033[0m \033[4mderived_class\033[0m\n", " ");
		printf("%5s\033[1mc+++ -getter\033[0m \033[4mclass_name\033[0m [\033[4m...\033[0m]\n", " ");
		printf("%5s\033[1mc+++ -function\033[0m \033[4mclass_name\033[0m [\033[4m...\033[0m]\n", " ");
		printf("%5s\033[1mc+++ -exception\033[0m \033[4mclass_name\033[0m [\033[4m...\033[0m]\n", " ");
		printf("%5s\033[1mc+++ -main\033[0m \033[4mmain_file_name_to_create\033[0m\n", " ");
		printf("%5s\033[1mc+++ -make\033[0m \033[4mprogram_name\033[0m \033[4mroot_directory\033[0m\n\n", " ");

		/* DESCRIPTION */
		printf("\033[1mDESCRIPTION\033[0m\n");
		printf("%5s\033[4mc+++\033[0m is a powerful tool to help your c++ project.\n\n", " ");
		printf("%5sThe \033[1moptions\033[0m are as follows:\n\n", " ");

		/* OPTION START */
		printf("%5s\033[1m-S\033[0m%5s\033[0mCreate a c++ file. No extension is required. Interface classes,\n", " ", " ");
		printf("%12s\033[0mabstract classes, classes, and function files are automatically\n", " ");
		printf("%12s\033[0mcreated separated according to naming rules. The same is true of\n", " ");
		printf("%12s\033[0mboth header(.hpp) and source(.cpp) files. Just enter name.\n\n", " ");

		/* OPTION START+ */
		printf("%5s\033[1m-S+\033[0m%4s\033[0mPerfectly the same as the -S option. However, it creates\n", " ", " ");
		printf("%12s\033[0mclass files in the \".class.hpp\" and \".class.cpp\" styles. However,\n", " ");
		printf("%12s\033[0mif you use this style, you must manually enter up to .class as a\n", " ");
		printf("%12s\033[0mfactor when using other options. I'll update that later, but not now.\n\n", " ");

		/* OPTION INHERIT */
		printf("%5s\033[1m-I\033[0m%5s\033[0mSet inheritance relationship by base class and derived class\n", " ", " ");
		printf("%12s\033[0mas arguments. Base class header include, modify canonical form\n", " ");
		printf("%12s\033[0mof derived class, inherit virtual functions of base class, etc.\n\n", " ");

		/* OPTION GETTER */
		printf("%5s\033[1m-G\033[0m%5s\033[0mSet the getter function for the variables set in the private\n", " ", " ");
		printf("%12s\033[0mand protected areas to header file and source file.\n\n", " ");

		/* OPTION FUNCTION */
		printf("%5s\033[1m-F\033[0m%5s\033[0mUpdate the member functions of the header file to the source file.\n", " ", " ");
		printf("%12s\033[0mJust open the source file and populate the body. Because the member\n", " ");
		printf("%12s\033[0mfunction is likely to continue to be added in the middle of the project,\n", " ");
		printf("%12s\033[0monly the functions ending in ';;' are updated.\n\n", " ");

		/* OPTION EXCEPTION */
		printf("%5s\033[1m-E\033[0m%5s\033[0mCreate an exception into an inner class with a canonical form\n", " ", " ");
		printf("%12s\033[0mwithin the definition of the class header file. The source files are also\n", " ");
		printf("%12s\033[0mupdated to match the canonical form. Some new grammar is required when\n", " ");
		printf("%12s\033[0mdefining an exception in a header file, which must be declared\n", " ");
		printf("%12s\033[0m'\033[1mexception\033[0m \033[4mexception_name\033[0m;' to run the option normally.\n\n", " ");

		/* OPTION MAIN */
		printf("%5s\033[1m-M\033[0m%5s\033[0mCreate main header and source file. Class files are not\n", " ", " ");
		printf("%12s\033[0mexplored, only prototypes of function source files are extracted. use a\n", " ");
		printf("%12s\033[0mdifferent name to avoid being overwritten. It is also useful for test program.\n\n", " ");

		/* OPTION MAKE */
		printf("%5s\033[1m-M+\033[0m%4s\033[0mMake file. You can specify the location of the source folder.\n", " ", " ");
		printf("%12s\033[0mUse '.' to base on the current folder, otherwise write down the name\n", " ");
		printf("%12s\033[0mof the folder. Put '/' after the name of the folder as much as possible.\n", " ");
		printf("%12s\033[0mNavigate only to 2-depth.\n\n", " ");

		/* IMPLEMENTATION NOTES */
		printf("\033[1mIMPLEMENTATION NOTES\033[0m\n");
		printf("%5s\033[1mC\033[0m is procedural-oriented and simple. But \033[1mC++\033[0m is object-oriented and a little\n", " ");
		printf("%5smore complex. What does this difference mean? In C, if you have simply written\n", " ");
		printf("%5sa story in your head, in C++, you must first create the world and its characters in\n", " ");
		printf("%5swhich the story will be made. Just as you don\'t exist, the narrative is carried out by\n", " ");
		printf("%5sthe interaction between the character and the world. Yeah. That\'s what the \033[1;4mobject\033[0m\n", " ");
		printf("%5smeans. Hiding you as a developer in the interaction of sophisticated objects.\n\n", " ");

		printf("%5sYou have to be responsible for many things, including the state, function, creation,\n", " ");
		printf("%5scopy, and destruction of objects, but once you've designed the objects well, a lot\n", " ");
		printf("%5swill be easier. The problem is that the design of the object is extremely troublesome,\n", " ");
		printf("%5scomplex and cumbersome. omg... Fortunately, if you understand the grammar of C++\n", " ");
		printf("%5sand deal with this tool, it will give you wings on your back without taking away\n", " ");
		printf("%5sthe strength of your legs.\n\n", " ");

		/* ERRORS */
		printf("\033[1mWARNING\033[0m\n");
		printf("%5sObjects are half designed. Do programming at the end. When all objects are\n", " ");
		printf("%5sdesigned, turn on C+++ and execute the commands in the order of start, inherit,\n", " ");
		printf("%5sgetter, exit, function, main, and make. If C++ is used with some development\n", " ");
		printf("%5sprogress without starting with design, the expected behavior may be different\n", " ");
		printf("%5sfrom that expected. Exceptions can occur in a\n\n", " ");
		printf("%5svariety of cases, such as entering an incorrect file name, applying other\n", " ");
		printf("%5soptions to files that were not created with the start option of C++, or\n", " ");
		printf("%5sclearing certain flags. Commit before using the possible C+++.\n\n", " ");

		/* BUF OR IDEA*/
		printf("\033[1mBUG OR IDEA\033[0m\n");
		printf("%5sIf you find a bug or have any ideas, send a \033[1mDM\033[0m to \033[1;4meunhkim\033[0m from Born to be slack.\n\n\n", " ");

		/* TAIL */
		printf("%-30s%30s%30s\n\n", "C+++", "\033[5;30;47m  AUG 6 2020, Ver 1.0  \033[0m", "C+++");
		return ;
	}
}

namespace help {
	void main()
	{
		print();
		return ;
	}
}
