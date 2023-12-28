# Writing An Interpreter in C++

Follow me as I write an interpreter in C++ and cry every day 😭😭😭.

The project is an interpreter for the Monkey programming language from the book
[Writing An Interpreter In Go](https://interpreterbook.com/).

## My History with C++

I learned C++ just like how normal CompE kids do, which is in second year
University. It was my first "serious" programming language when I didn't know
any better how much easier it could be. After finishing
[my first C++ big school project](https://github.com/cbebe/Checkers-AI), I
learned JavaScript and never looked back at C++.

I learned more about programming fundamentals after that, and how computers work
in general. I tried picking up C++ again in 4th year for my first Operating
Systems assignment and quickly got overwhelmed. I ended up doing the assignment
in C, which I found way simpler.

I ended up doing the last assignment in C++ (it was the only other option for a
programming language) because I was too lazy to implement my own hashtable in C,
but I kept it C-style with only a little bit of OOP sprinkled in.

## Why C++ Now

- I'm a big boy programmer now who can use StackOverflow and ChatGPT
- To remind myself how much better "modern" programming languages are (EDIT:
  modern C++ is okay too)
- To learn new C++ features since I haven't seriously touched it in almost 3
  years (if I'm being honest, I have never seriously touched C++)

## Thoughts

This was painful. I already felt comfortable by the time I reached the start of
the evaluating chapter (with `std::unique_ptr`s and whatnot), but things got a
bit hairy when I started dealing with functions. I basically just changed all
instances of `std::unique_ptr` and used `std::shared_ptr`. Even after finishing
the book, I wasn't able to fix the memory leak when binding functions to
variables.

As for learning C++, I think I've had enough of it. I would probably be less
intimidated to read and write C++ now. I'd pick it for a school project if I had
to choose between C or C++, but it definitely is still not a top choice for me.

I didn't really get to learn how to use CMake and testing frameworks but it's
not like ~~I'll be starting new projects in C++ anyway~~ (EDIT: I am using CMake
and it's painful). I'm going to write the
[compiler and virtual machine in Rust](https://github.com/cbebe/monkey-rs).
