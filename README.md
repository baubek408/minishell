# Minishell (42 School Project)

This project is a custom implementation of a basic Unix command-line interpreter (shell), similar to `bash`. It was developed as part of the curriculum at 42 School.

## Core Features

* Displays a prompt and reads user input using the Readline library.
* Maintains a command history.
* Parses complex commands, arguments, and operators.
* Executes external commands found via the `PATH` environment variable, or using relative/absolute paths.
* **Pipelines:** Supports chaining commands using pipes (`|`).
* **Redirections:** Implements input/output redirections:
    * `<` : Redirect standard input.
    * `>` : Redirect standard output (overwrite).
    * `>>` : Redirect standard output (append).
    * `<<` : Here-documents.
* **Environment Variables:** Handles expansion of variables (`$VAR`, `$?`) and respects single (`'`) and double (`"`) quotes.
* **Signal Handling:** Manages `Ctrl+C` (new prompt), `Ctrl+D` (exit shell), and `Ctrl+\` (ignored).
* **Built-in Commands:** Includes the following essential built-ins:
    * `echo` (with `-n` option)
    * `cd`
    * `pwd`
    * `export`
    * `unset`
    * `env`
    * `exit`

## Build Instructions

1.  Clone the repository (if applicable).
2.  Navigate to the project directory.
3.  Run `make`:
    ```bash
    make
    ```
4.  This will create the `minishell` executable.

Available `make` rules:
* `make all` or `make`: Compiles the project.
* `make clean`: Removes object files.
* `make fclean`: Removes object files and the `minishell` executable.
* `make re`: Re-compiles the project clean (`fclean` + `all`).

## Usage

To start the shell after compilation, simply run:

```bash
./minishell
