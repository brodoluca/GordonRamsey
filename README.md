# GordonRamsey
Github repository of the project for Distruibuted Systems, Hshl, 6th semester.
Team members:
    Luca Brodo
    Kaan Tuna
    Gordan Konevski
    Wiktor Kochanek
    
## Work in progress
Now im doing the JSon parser. I know there are libraries that already do that but I prefer writing my own. First of all, I understand how it works. 
Secondly, why should I learn another library which will not have any documentation? I already did that and it was a mistake.
Finally, I can tailor the parser to benefit the code. The thing is, I dont think im gonna need a lot. 
I need for sure:
-Integer
-Float
-Strings

it would be nice to have
-Array
-Objects

However, these will be only a commodity. I dont care about them really. Sending objects, especially caf objects,  will be a pain in the ass, 100%. What we can do, however,in case we manage to send messages,  is send the connection handles around and this will save a lot of complexity, I think.
I dont know, I will figure this part later on. 


## Idea 
The idea is to treat every truck as if it was a node in a linked list. Every truck will have a server and a client part (For a lack of a better term). It wall us the client to connect to the car in front and the server to allow the truck behind to connect to itself. If we treat the platoon as a linked list, we can manage to abstract the whole thing, including operations like resizing, and to estimate the performace as well. 
## Installation
Use CAF

## Getting Started
Nothing do to really, install caf and build this thing


## Versioning
    Eclipse 2020-03 with Papyrus installed
    XCode 11.5
    Caf 1.8.2
    C++ 17


## License

We do not think that in the current situation the project, code in particular, is good enough to be used in a real situation. Please contact the owner of the repository if some parts of the code are going to be used.
[MIT](https://choosealicense.com/licenses/mit/)








