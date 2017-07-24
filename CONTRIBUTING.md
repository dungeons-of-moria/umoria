# Contributing to Umoria

Thanks for your interest in contributing to the Umoria project!

The following is a set of guidelines for contributing to Umoria, which is hosted in the [Dungeons of Moria organization](https://github.com/dungeons-of-moria) on GitHub. The following are mostly guidelines, not rules. Use your best judgement, and feel free to propose changes to this document in a pull request.


## Code of Conduct

This project and everyone participating in it is governed by the [Umoria Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to [info@umoria.org](mailto:info@umoria.org).


## Questions and Discussions

Currently the Umoria project does not have its own blog or community forum, however, if you wish to discuss the game or ask general questions about Umoria, the best place to start would be the [MoriaRL Reddit page](https://www.reddit.com/r/moriarl/). You'll get faster results than by using the resources below.

For more general questions and discussions on the Roguelike genre, please visit the [Roguelike Reddit page](https://www.reddit.com/r/roguelikes/).

These are both friendly and welcoming communities.


## What should I know before I get started?

First, and most importantly, the current goal of the project is to not implement new gameplay changes or features to Umoria. The gameplay was fine-tuned over a period of many years and has been in its current state for more than 20 years.

Perhaps at a future date gameplay changes will be considered, but for the moment it should not change.

The focus of current work has been on cleaning up the code base; using _modern_ C/C++ coding styles, updating to compile against C++14, breaking up the many large and complex functions to create smaller more manageable blocks of code. Along with making the code easier to understand these changes will also make any future features easier to implement. One idea is to switch from NCurses to SDL to allow mouse support to be added.


## How Can I Contribute?

### Reporting Bugs

This section guides you through submitting a bug report for Umoria. Following these guidelines helps maintainers and the community understand your report, reproduce the behavior, and find related reports.

Before creating bug reports, please check out the [current issues](https://github.com/dungeons-of-moria/umoria/issues) as you might find out that you don't need to create one. When you are creating a bug report, please include as many details as possible, as indicated the next section.

> **Note:** If you find a **Closed** issue that seems like it is the same thing that you're experiencing, open a new issue and include a link to the original issue in the body of your new one.


#### How Do I Submit A (Good) Bug Report?

Bugs are tracked as [GitHub issues](https://github.com/dungeons-of-moria/umoria/issues). Create an issue explaining the problem, and including any additional details that you think may help maintainers reproduce the problem:

* **Use a clear and descriptive title** for the issue to identify the problem.
* **Describe the exact steps to reproduce the problem** (if possible) with as many details as you can.
* **Describe the behavior you observed after following the steps** and point out what exactly is the problem with that behavior.
* **Explain which behavior you expected to see instead and why.**
* **If you're reporting that Umoria crashed**, include a crash report with a stack trace from the operating system. On macOS, the crash report will be available in `Console.app` under "Diagnostic and usage information" > "User diagnostic reports". Include the crash report in the issue in a code block, a file attachment, or put it in a [gist](https://gist.github.com) and provide a link to that gist.

Include details about your configuration and environment:

* **Which version of Umoria are you using?** You can get the exact version by running `umoria -v` in your terminal. If you compiled directly from the source code, please state that.
* **What's the name and version of the OS you're using**?
* **Are you running Umoria in a virtual machine?** If so, which VM software are you using and which operating systems and versions are used for the host and the guest?
* **Which keyboard layout are you using?** Umoria or traditional Roguelike keys (`hjkl`)?


### Code Contributions

General points worth remembering before making your Pull Request (PR):

* Avoid platform-dependent code.
* Format your source code to the provided [`.clang-format`](.clang-format) style.

At present there are no strong style requirements, but here are a few ideas that I would like to start thinking about.

#### General Formatting

* Indentation: one indent should be *4 spaces*, so please be careful to avoid the use of _tabs_.

#### Use Standard Types

At present I'm using only _standard types_, so please do continue this practice.
The common standard types are:

* `int8_t`, `int16_t`, `int32_t`, `int64_t` — signed integers
* `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t` — unsigned integers
* `float` — standard 32-bit floating point
* `double` - standard 64-bit floating point

When representing ASCII characters we should be using the `char` type.

#### Naming

* **Classes / Structs / Types**: `CamelCase`, with the first character uppercase.
* **Functions**: `camelCase`, with the first character lowercase.
* **Variables**: `snake_case`, and all lowercase.

I like the easy visual distinction from naming like this. You can see immediately what its function is and without having _noisy_ suffixes.



