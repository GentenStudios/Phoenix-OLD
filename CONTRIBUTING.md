# GENTEN STUDIOS CONTRIBITING GUIDELINES
These are mostly guidelines, not rules. Use your best judgment, and feel free to propose changes to this document in a pull request.

## TABLE OF CONTENTS

[GITHUB GUIDELINES](#GITHUB-GUIDELINES)
* [GIT FLOW](#GIT-FLOW)
* [PULL REQUESTS](#PULL-REQUESTS)

[C++ CODING STANDARDS](#C++-CODING-STANDARDS)

## GITHUB GUIDELINES
### GIT FLOW
On GitHub, we use a practice commonly known as Git-Flow. You can read about the process in depth on [Vincent Driessen's Guide](https://nvie.com/posts/a-successful-git-branching-model/), off of which we base our current workflow. If at any point that guide conflicts with what is written here, the methods & instructions specified here should be used.

Familiarity with Git, specifically with using branches, is a must for interacting with Genten Studios' repositories and is a prerequisite for learning the Git-Flow process. The process is based on separating the project into categories of branches as follows: master, release, development, feature branches, and hotfixes.

**The Master Branch** is the *most* stable source for the code, this contains major/ minor releases and nothing else. Only release and hotfix branches can be pushed here.
- All Genten Developers and Staff Members must approve any push to the master branch - unless said otherwise (e.g. last minute hotfixes).
- The code must be thoroughly tested on all supported systems before being pushed here.

**Hot Fix Branches** are a method for pushing code to the master branch after a release. These should be minor fixes that add no functionality (unless the bug restricts a certain functionality) to the project.

**Release Branches** are the staging area for a release. Similar to a hotfix branch only bug testing is to be done here and no new features should be added.
- A 3/4 developer approval to push to the release branch.
- The code must be tested on all supported systems before being pushed here.

**The Develop Branch** Is where new features are added, however not where they are worked on. Commits should be merged from a feature branch into the develop branch and from nowhere else.
- It takes a greater than 2/3 developer approval to push to the develop branch.
- The code must be tested on all supported systems before being pushed here.

**Feature Branches** are where the action happens, each feature branch contains the addition of a single new feature. This is where all main development work happens and it is required to create a feature branch to push code. Any Genten developer can accept code into a feature branch.
- These branches must follow the naming convention of ``feat-[feature name]`` (e.g. ``feat-voxels``)
- Sub branches will generally follow a similar naming convention of ``feat-[feature name]-[subfeature]`` (e.g. ``feat-voxels-blocks``)

### Pull Requests
- A pull request from a contributors repository can be approved by a single developer, this code can only be pulled into feature branches and not directly into the development or higher branches.

### PR checklist
This section lists the requirements for merging code. All of these requirements must be satisfied before code is merged into any branch.

All Branches:
- Accomplish the feature(s) it was designed to accomplish
- Has the branch it's merging into merged onto itself and all conflicts are resolved
- Clean of all binaries and other non-source material
- Code is documented with doxygen style comments
- Complies with style guide and other contributing guidelines
- All Developer reviewer comments are resolved
- Code is formatted with cLang

Development branch:
- Compiles properly on all supported systems

### VERSIONING AND RELEASES
Versioning will follow a standard of MAJOR.MINOR.FIX where Major Releases mark significant points in a product’s lifetime and Minor Releases mark the finalization of significant features. Fixes and non-feature releases are ones that repair issues with code.

Each project will have a drafted project plan/roadmap that marks each Major and Minor release scheduled in advance throughout the development life cycle.

Each project moves to an Alpha state when it is ready for community contributions, then a beta state when it is ready for playtesting. These states do not have to correspond to a specific version number but should occur before 1.0 during a Minor Release. Each project will reach a released state at 1.0, this is where the product is ready to be played regularly.

## C++ CODING STANDARDS

Our ethos consists of the idea that we're writing code for other people. This means we want our code to look tidy, professional, and function; to achieve this, we've had to implement some standards - which we could greatly appreciate a following to when submitting Pull Requests and the likes.

### NAMING
Consistency is key - the key to easy, readable code - especially since our code is not only for us. Naming consistency will help identify different types of variables, constants, files, methods, and more. Consistency - however - is not the final stop... Something needs to be named as per it's functionality or it's use; it should be self-explanatory, you shouldn't have to ask to understand what it is.

#### FILES
All files must follow the `PascalCase` convention, where each “word” in the file name has a capital on its first letter and no spaces. All C++ files must use either `.cpp` or `.hpp` for source and header files respectively.
For example, this type of convention must be followed:

```
PascalCaseFileName.hpp
PascalCaseFileName.cpp
IWindow.hpp
IWindow.cpp
```

#### CLASSES
Classes must also follow the `PascalCase` naming convention, to allow for faster reading, and identification of classes within code. For example:

```cpp
class FooBar
class Caterpillar
```
#### FUNCTIONS
Functions should follow the `camelCase` convention, where the first word has a lowercase first letter, but all consequent words start with a capital letter, like so:

```cpp
int doSomething();
int goddIsTodd();
int wowThisIsACoolFunction();
```
#### VARIABLES
There are a few things to know about naming variables. Most all variables must follow the camelCase convention, however, private member variables of a class must use an `m_` prefix before them. However, constants and macros should be ALL_CAPITALS_USING_UNDERSCORES_INSTEAD_OF_SPACES. Hopefully that makes sense, if not, here’s an example to make things make sense:

```cpp
const int THIS_IS_A_CONSTANT = 10;
int g_globalVariableForFooQuality = 5; // Global variable, uses the g_ prefix

class FooBar
{
public:
    int doThing(std::string descriptionOfThing); // Parameters also use camelCase.
    float randomNumber; // This is a normal public variable, uses no prefix.
private:
    int m_fooQuality; // This is a private member variable, uses the m_ prefix.
};
```

Do not use any hungarian notation, as we internally have found that redundant, and truly a divergence from our goal to become super saiyan.
### MISC. NAMING
Namespaces must start with lowercase characters, however, whenever creating another namespace, consult the core developer first, as overuse of namespaces can become triggering.
## COMMENTING
For any comment that requires only a single line, use the // method of commenting. Always make sure the // has a space before the actual comment. This is for easier readability, and no sudden fear to visit the opticians because we just became blind old grannies. For example:

```cpp
// This is a single line comment
int bar();
```
Multiline comments should use the /* */ method of commenting, where the /* and */ are on their own lines. Each line in between should house an asterisk, please ensure there is a space before following comment. For example:
```cpp
/*
 * This is a multiline comment
 * As in a comment with multiple lines.
 * Etc...
 */
```
When documenting classes, methods and similar we follow the Doxygen Javadoc syntax. For example, this convention should be followed:
```cpp
/**
 * @brief This class is for documentation reasons.
 */
class FooBar {
public:
	/**
	 * @brief This does an important thing
	 * @param desc A description of the thing
	 * @return A very important integer. (probably an error code or something)
	 */
  	int doThing(std::string desc);

  	float randomNumber; //< This is a single line description for a member

      /// @brief This member variable requires a longer single line comment. (filling space)
      bool foobarred;
};
```
Multiline Doxygen comments should start with a /** and end with a normal */. The second asterisk is something that Doxygen recogises through its own interpreter. There are two ways for single line comments, choose whichever one looks right, a slightly longer one however should use the second method, just so we don’t have to become grannies to read.
## MODULARISATION
Currently the code is separated by function, eg all the code for rendering is in one spot. Each module should sport a easy-to-use public API. This API should be as generic as possible in order to make the replacing, and use of code easier.
## TODO: FORMATTING, USAGE OF C++ FEATURES

# THIRD PARTIES
We use many third party libraries to support the functionality of our projects. While working with third parties there are a couple things to keep in mind:
### 1. Qualifications
When choosing a third party, we need to make sure the third party is right for our needs. You need to consider the license, how maintained the library is, how stable the library is, and if there are any alternatives better suited for what we need. It's best to reach out in #programming on our Discord to discuss the viability of a third party before selecting one. You don't want to have these questions unanswered when making your PR request.
### 2. Install
When working with C++ code, we should use a submodule for third parties if possible. We also want to keep the third party code seperate from our own source code so it should be placed in the ThirdParties folder of the project.
### 3. Never modify the third party
To help with maintainability, third parties should have no modified code in them. When it comes to updating the third party it should be as easy as replacing the current version with the new version.

#### </b> {#contributing}
