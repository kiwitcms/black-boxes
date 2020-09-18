# Hardware for teaching exploratory testing

The original idea comes from
[James Lyndsay](http://exercises.workroomprds.com/) with more recent
implementation from
[Claudiu Draghia](http://blog.brainforit.com/2017/02/06/black-boxes-for-black-box-testing/).

The premise of these exercises is that when analyzing/exploring a complex
system under test (SUT) it is not very easy to be exercising the system and
observing what happens and analyzing your own actions to figure out if you are
missing something or not. The goal of all exercises is to figure out how each
of them works, e.g. how each box works.

The term [black-box testing](https://en.wikipedia.org/wiki/Black-box_testing)
is used when we have to deal with a SUT whose internals are unknown to the observer.
The only way we can interact with SUT is by input values and the only feedback that
we get is observing if/how SUT changes its final state. All of the examples here are
designed with this in mind, hence the boxes are black in color.

The opposite term also exists:
[white box testing](https://en.wikipedia.org/wiki/White-box_testing) -
this is when SUT internals are known to us, either via documentation or source code
or schematics, something else and/or all of the above.
