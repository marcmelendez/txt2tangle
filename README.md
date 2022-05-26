# txt2tangle: A very simple literate programming tool

I create readable code with **txt2tangle**. Instead of
writing for a computer, I write for humans. Beginning with
an explanation in a plain text file (or perhaps a source
code file, such as a LaTeX document), I simply insert
**txt2tangle** commands on their own line, preceded by a
special string ("%!" by default, but that may be changed as
seen [below](#command-line-options)) and use them to mark
which parts of the text are intended as code.

For example, a document for a typical *Hello world* program
might look like this:

> ```
> === Hello, World! ===
>
> A "Hello, World!" example program.
>
> %! codefile: helloworld.bc
> print "Hello, world!\n";
> quit();
> %! codeend
> ```

From this text file, **txt2tangle** would create the
`helloworld.bc` file.

As a matter of fact, the files for this project were
created by **txt2tangle** from just two plain text files:
`txt2tangledoc.txt`, which contains the information to
generate this README file and the Makefile, and
`txt2tangle.txt`, which contains the C code to make the
**txt2tangle** executable file.



## How to use **txt2tangle**

For a simple tutorial explaining how to write code with
**txt2tangle**, refer to the `txt2tangledoc.txt` text file
that generated the code for this `README.md`. To see how
**txt2tangle** works, you can either read the C code
directly (`txt2tangle.c`) or read the more literate
`txt2tangle.txt`file that generated it.


## **txt2tangle** commands

* `%! codefile: <filename>` opens a source code file for
output.
* `%! codeend` marks the end of the source code file.
* `%! codepause` stops sending output to the code file.
* `%! codecontinue: <filename>` resumes output to the file
specified.
* `%! codeblock: <blockname>` specifies a block of code.
* `%! codeblockend` marks the end of a block of code.
* `%! codeinsert: <blockname>` or
`%! codeinsert: <blockname> src: <filename>` inserts the
specified block from the source file mentioned. If no
source file was named, the current file being read is
assumed.
* `%! codedefinition: <blockname>` or
`%! codedefinition: <blockname> src: <filename>` behaves
just like `codeinsert` with one simple difference: the
path of the source file is interpreted relative to the
parent calling block.

## Command-line options

The only command-line option is `-c`, which allows you to
change the default string marking the beginning of a
**txt2tangle** command. You could write, for example
``txt2tangle -c // example.txt``
and the program would identify strings such as
``// codefile: out.c``
and
``// codeend``, for example.

## Compiling

After cloning the repository, compile with `make first`.
From then on, you can compile by just running `make`.


## **txt2tangle** inside other code files

In addition to using **txt2tangle** with regular text files
to write text and code, you could combine its instructions
with other source codes. For example, you could include
your code with an explanatory LaTeX document or, conversely,
you might prefer to include LaTeX commands within your C
source code, which can then be extracted to create the
documentation. A few examples follow.

### txt2tangle + LaTeX

An attractive way to produce code and well typeset
documentation involves writing everything in a single LaTeX
file. A simple solution based on the **listings** package
includes the **txt2tangle** code in the code listing and
specifies them as comments not to be reproduced in the
text document. A minimal example follows.

>```LaTeX
> \documentclass{article}
> \usepackage{listings}
>
> \begin{document}
> \lstset{morecomment = [is]{\%!}{\^^M}}
>
> Here is a ``Hello, World!'' program written for the bc
> command line calculator.
>
> \begin{lstlisting}[frame=single]
> %! codefile: helloworld.bc
> print "Hello, World!\n";
> quit();
> %! codeend
> \end{lstlisting}
>
> \end{document}
>```

### txt2tangle + C/C++

Easily include code within C multiline comments. Remember
that **txt2tangle** instructions must be the first thing on
a line (apart from whitespace). Running **txt2tags** on the
text example below would produce two new files: `Makefile`
and `sine.gnuplot`. Assuming you have **gcc**, **make** and
**gnuplot**, running the `make` command would compile, run
and represent the output graphically.

>```C
> /* sine.c: sin(x) values in the interval [0, 2 pi] */
>
> # include <stdio.h>
> # include <math.h>
>
> /* Makefile to compile and run this code
> %! codefile: Makefile
> all:
> 	gcc -Wall -o sine sine.c -lm
> 	./sine > sin.dat
> 	gnuplot sine.gnuplot
> %! codeend
> */
>
> int main(int argc, char * argv[])
> {
>   /* Output data for a sine function in the interval [0, 2 pi] */
>   float x;
>   for(x = 0; x < 2*M_PI; x += 0.02)
>     printf("%f %f\n", x, sin(x));
>
>   /* Gnuplot code to represent the function graphically
>      (run with gnuplot sine.gnuplot)
>
>      %! codefile: sine.gnuplot
>      set title "Sine function"
>      set xlabel "x"
>      set ylabel "sin(x)"
>      plot [0:2*pi] "sin.dat" w l notitle
>      pause -1
>      %! codeend
>   */
>   return 0;
> }
>```

### **txt2tangle** + txt2tags

An easy way to combine **txt2tangle** with txt2tags just
commands the preprocessor to eliminate all **txt2tags**
instructions before parsing, as below.

>`````
> txtweb: a simple DIY literate programming tool
> M. Meléndez
> %%date(%d/%m/%y)
>
> %! preproc: '%!([ \t]*)codefile:(.*)$' ''
> %! preproc: '%!([ \t]*)codeblock:(.*)$' ''
> %! preproc: '%!([ \t]*)codeinsert:(.*)$' ''
> %! preproc: '%!([ \t]*)codepause(.*)$' ''
> %! preproc: '%!([ \t]*)codeend(.*)$' ''
>
> =txt2tangle + txt2tags example=
>
> A //Hello, World!// example should suffice to
> explain how to use **txt2tangle** in combination
> with **txt2tags**.
> ```
> %! codefile: helloworld.bc
> print "Hello, World!\n"
> %! codeend
> ```
>`````

