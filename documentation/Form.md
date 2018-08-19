Form
====

Data & data types
-----------------

There are x basic data types in Form: signed integers (32 bit), 32 bit floats,
strings, boolean, variables, the special marker, functions (named or unnamed), arrays, and objects.
 
All data is either pushed to the global data stack (which is shared among all functions and all scopes), stored inside an array, and object, or a variable. 

There are no references in Form. If an object is duplicated, it is a deep copy. The only exceptions are functions. Because of this, memory managment is not a concern. Scopes will free variables, and memory can be dynamically used via the stack.

To push data to the global data stack, either have a literal in the place of code you want to add a literal, create a composite data type, or call a function that pushes data to the top of the stack


```
// pushing a literal
3

// composite
{ 4 2 s }array

3 dup

```

The special marker
------------------

The value "{" will push a marker to the stack. "{" is equal in value only to another "{", and false if compared for equality to all others.

The purpose of the marker is to mark the end of an interderminate range of the stack.

Creating Functions
------------------

To declare a named function:

```
:myFunction
	1 5 +
;
```

:>name< denotes the name of the function. ; denotes the end of the function.
There are no return variables, nor parameters; functions must take information from the stack, and push computed data to the stack.

The name of a function can begin with any letter or '}'; the following characters may be alphanumeric characters, or '_'.

The key word `return` will prematuraly end a function. It is similiar to a `break` works for loops in other languages.

Form supports recursion.

An anonymous function can be declared as:
```
:
	1 5 +
;
```
This will push a function to the top of the stack.

Named functions are scoped to the their block. They can not be called outside the block they are declared.

Calling Functions
-----------------

To call a named function, the function must first be pushed to the data stack, and then called with @.

```
myFunction @
```

To call unnamed functions, the function must be on the data stack, and called with @.
```
: 1 5 + ; @
```

Branching
---------

Branching is done with five keywords:
- if
- elif
- then
- else
- endif

All conditions look at the top of the stack, and determine if the value is truthy (0, false, null, { }array, { }object, "", 0.0 are false, everything else is true). Syntax looks like one:

```
>"condition"< if
	>executed if top of stack is true< 
	>(jump to endif)<
then >"condition"< elif
	>Executed if top of stack if true<
	>(jump to endif)< 
else
	>Executed if all other conditions are false<
endif
```

Iteration
---------

Form currently only supports while loops.

A while loop starts with a "do" keyword and ends with a "repeat" keyword.
When "do" is reached, execution is automatically jumped to the word after.

If a "while" keyword appears in the same scop as the do...repeat block, execution will immediately after the ending "repeat" keyword if the top of the stack is false. Else, the next word is executed 

Example:
```
1
do dup 5 == while
	1 +
repeat
```

Variables
---------

Variables are scoped, and are pushed to the stack. A variable literal is a dot followed by a letter, then any amount of alphanumeric characters, -, and _. 

To save a value to a variable, use the = keyword. The variable to save to must be at the top of the stack, and its data second on the stack. This operation removes both the variable and the value on the stack; however, pushing a variable with the same name and retreiving its value will push the value stored earlier with the = operation.

If the value being assigned to a variable is another variable, the assignee will be have its copy valued. Variables can not store variables.

The @ operator reads the variable from the top of the stack, and pushes the variable's data to the top of the stack, and remove the variable from the stack.

If a variable is created and its value retrieved without one ever being set, execution mumst end.

Arrays
------ 

There are two ways to create an array.

The command `}array` will look at each item on the stack until it reaches a marker. Each item before the marker will become part of the array, with the top of the stack being the last item in the array.

If no marker is found, stack overflow occurs. If no values come before a marker on the stack, the array is empty.

Alternatively, if the "array" keyword is called while a non-negative integer is on the top of the data stack, an array of that size will be created. Values may be set to each index and later retrieve, but retreiving any item in any other index will end execution.

To access an element at an index, use the `[]` operator while an integer is on the top of the data stack, with the array to access second. Altenertively, [x], with x being a non-negative integer, will also access the element at the xth index of an array present on the top of the data stack.

Objects
-------

# Object creation
And object is created with either the operator `{}`, or the keyword `object`. This will create an empty object.

# Member access and assignment
To access a member in an object, have a variable at the top of the stack, the object beneath it, and use the operator `.@`. This will push the value of the member to the top of the stack. The variable will be consumed; however, the object will. If the member can not be found on the object, execution will end.

To assign a member to an object, have a value on the top of the stack, follow by the variable and object. Use the operator `.=`. This will consume the value and variable, but not the object. You do not need to explicitly define member fields on objects before hand. If a variable is to be assignen, the variable's value will be copied instead. Members can not store variables.

# Method access
To access a member in an object, have a function name at the top of the stack, the object beneath it, and use the operator `.@`. This will execute the method found on the object. The function name will be consumed BEFORE execution; however, the object will remain on the top of the stack.

There are multiple ways to assign a method to a function.

If you have a named function on the top of the stack and an object below, using the `.=` operator will set the function to the method of the object. This method can be used, even outside the scope of where the function was defined.

If you have a function name OR variable name, an anonymous function beneath it, and an object beneath it, calling the `.=` operator will assign the anonymous function as a method with the name of the function name or the variable name. 

# Prototyping
Form uses prototyping. If attempted to access a method or member of an object who has never had the method or member accessed, it attempt to access the method or member on its prototype object, if it exists. The prototype of the object can not be accessed directly.

To set a prototype object, have the object to be a prototype on the top of the stack, with the base object below. The `new` keyword will set a copy of the object on the top of the stack to be the prototype of the second object. This will consume the top of the stack.

Please remember there are no references in form. All prototypes are unique objects. Re-accessing the prototype of an object via a variable and modifying it will not change the prototypes that have that such object. 

(May change new so it creates an empty object with the prototype set)

Comments
--------
( ) are treated as whitespace.

// is a single line comment
/* */ are multi-line comments
/word will turn a single word into a comment.

Keywords
--------

Comming soon

Operations
----------

Coming soon

Coming soon
-----------

Better detail on terminology, such as 'word', 'operation', 'execution', etc.

Terminology relating to describe the stack.

Error stuff.

Behavioural details. Like crashing.

"data literals", "words", "operators", "function declarations".