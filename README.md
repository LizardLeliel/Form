Form
====

Form is a stack-oriented language, currently in the works (Postword was this language's prototype). At the moment, it does nothing.
Features are coming; but at the moment, back-end stuff for the language is still being devolped
(hashes, byte-code intrepretation, etc.). So, more is to come.

Although if you wish to compile and run it in its current state, you should read this:

Some files are excluded from the repository; namely sample input
and "lex.yy.c", the file generated from "formLang.flex". If you wish to build this
program from a clone on your computer (Even though its nowhere near completition), 
type "sudo apt-get install flex" into the command bar, go the the clone's root directory,
and type "flex flexLang.flex". lex.yy.c will be generated, and will be able to be linked.
You'll also need an input file of giberish; although it will tell you if it found an integer
and will say it found a function if you have something in the form of :(whitespace)*(variableName-by-C-rules)

Todo: <br>
- [ ] Get it working
- [ ] Make a make file
- [ ] Backus-Naur documentation (?)
- [ ] A guide to the language
- [ ] A proper readme.md
