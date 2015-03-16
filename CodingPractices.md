# Coding practices #

This will serve as a quick tutorial of good coding practices, to allow for better software/hardware development as a group. This will cover subversion, our version control system, and Makefiles.

# Subversion #

Subversion is a relatively simple version control system that we use and our repository is hosted at: http://code.google.com/p/mda-ut/.

A version control system (VCS) is great at tracking changes and sharing a project codebase within a team. You need to worry less about 'Does my new change work?', since if it doesn't, changes can always be rolled back. Even so, you should at least check that the code builds properly (see next section on Makefiles). Some quick tips about general VCS usage:

Do
  * Commit minor tweaks, even if it just fixes 1 compile warning.
  * Commit often. I would commit about 1-2 hours of work. 2-3 sentences in the comment should be enough to describe the entire change. Further details can be added, but should not be necessary to understand the change
  * Add readme text files and test cases (if applicable) to the repository

"But wait, I don't understand some of the terminology that was just written." If that's you, here's a quick glossary

  * Commit: applying a set of changes to one or more files to the repository. Each commit is stored as a revision on the repository.
  * Repository: where the actual code is stored on the server, including all version information.
  * Revision: (hopefully) a version of the code that works, but may not be completed. Getting the code at a specific revision is relatively easy.

Now the fun part, the important commands:

  * `svn help`: help! For help on a specific command, try `svn help "<command>"`.
  * svn checkout: Download a copy of the latest code from the repository to your local drive. Note, you should only need to do this once.
  * svn update: Update a checked out (existing) local build to the latest version, integrating other people's changes.
  * svn add: Add a new file to be tracked by svn. When you commit a change, this file will be added to the repository.
  * svn delete: Delete a file so that it is no longer tracked by svn. Note, a history of this file will still be maintained in version history.
  * svn status: See which files you modified before committing.
  * svn commit: Add your current changes to the repository. You must add a descriptive comment to this change.

More advanced, but useful commands:

  * svn diff: Get a diff of your current changes against the repository (try `man diff` if you don't know what a diff is).
  * `svn log`: Read the comments of the latest commits.
  * `svn revert`: Undo an `svn add` or `svn delete` command. This will also undo a modified file and revert it to the repository file.

"That's great, now how do I commit code?" Just follow these steps and they will work 90% of the time.

  1. `svn update`: You can only commit changes if you have the latest version of the build.
  1. `make`: You want to ensure your code still compiles.
  1. `make clean`: Once you know your code compiles, remove the files generated by the build process. The repository only needs to store source files.
  1. `svn status`: This will show you what files you will submit. Double check these are only files that you meant to change.
  1. `svn add`: any new files that should be tracked. `svn status` will show a ? beside this file name if it's not currently being tracked.
  1. `svn commit`: This may open up a scary editor and ask for a comment. If you don't know how to use this editor, try `svn commit -m "<comment>"` instead.

Now, off you go and commit some changes to the repository! It's okay to make a mistake since someone will be watching new commits to ensure they work.

# Makefiles #

Makefiles are a way to automate build processes, most commonly to compile and link code. This is very simple, all you have to do is type make in your subsystem. So how does this work? make looks for a file called Makefile in the current directory, and executes commands written in the Makefile. You may need to modify the Makefile if you add new source files, but otherwise it should work and build your code properly so you can try out your changes.

And one more thing, you can type `make clean`, and that should remove all the non-source files including executables created from typing make. Files that are removed by 'make clean' should not be committed to the repository.

To find more about how to modify Makefiles, use your favourite search engine. This is a more advanced topic and is not covered by this basic overview.