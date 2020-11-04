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

> `=== Hello, World! ===`
>
> `A "Hello, World!" example program.`
>
> `%! codefile: helloworld.bc`
> `print "Hello, world!\n";`
> `quit();`
> `%! codeend`

From this text file, **txt2tangle** would create the
`helloworld.bc` file.

As a matter of fact, *all* the files for this project were
created by **txt2tangle** from just two plain text files:
`txt2tangle.txt`, which contains the information to
generate this README file, among others, and
`txt2tangle.txt`, which contains the C code to make the
**txt2tangle** executable file.


## **txt2tangle** commands

* `%! codefile: <filename>` opens a source code file for
output.
* `%! codeend` marks the end of the source code file.
* `%! codepause` stops sending output to the code file.
* `%! codecontinue: <filename>` resumes output to the file
specified.
* `%! codeblock: <blockname>` specifies a block of code.
* `%! codeinsert: <blockname>` or
`%! codeinsert: <blockname> src: <filename>` inserts the
specified block from the source file mentioned. If no
source file was named, the current file being read is
assumed.


## Command-line options

The only command-line option is `-c`, which allows you to
change the default string marking the beginning of a
**txt2tangle** command. You could write, for example
``txt2tangle -c // example.txt``
and the program would identify strings such as
``// codefile: out.c``
and
``// codeend``, for example.

