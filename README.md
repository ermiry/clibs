# clibs

**clibs** is a collection of my personal C libraries that I have created over the years, mostly from scratch, others using an already existing base or implementation and then worked to added some new features or refactor the code to best suit my needs.

These libraries are heavily used in Ermiry projects and some are the core components for frameworks such as Cengine and Cerver. They started just as simple implementations, but over time, new feaatures have been added to make them more reliable, easy to use, and versatile to fit almost every application.

Here you can find some custom implementations of common data structures such as a Single Linked List, a Double Linked List, an AVL Tree, a simple Stack and many others. The collection of data structures will continue to grow as more implementations are needed for Ermiry Projects. All of these have an exposed API with a set of methods that are really easy to use and have documentation and examples. You can also find some tests to check for yourself how they perform and what are their strengths and limitations. All of these data structures have been on constant testing and development, with the latest implementation of thread safe methods.

You can also find some other utilities, like c strings methods, a log and many more methods that have been of greater help in all of Ermiry projects.

You can find a greater explanaition of the libraries, as well as the documentation with some examples here: clibs.ermiry.com

## Usage

Once you have picked up what you need from the **src** folder, you also need to grab the corresponding header .h file from the **include** folder. The .h file contains the public methods declarations for the real implementation in the source code.

Most of them work out of the box, some others need the included **types** sources from this repo as well, and a few selected ones, more specialized, may required some extra installation, but all the instructions can be found in the documentation pages.