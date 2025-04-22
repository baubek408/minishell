# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bmynbyae <mynbyae@student.42prague.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/15 13:05:28 by bmynbyae          #+#    #+#              #
#    Updated: 2025/04/22 14:39:39 by bmynbyae         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# --- Executable Name ---
NAME = minishell

# --- Compiler and Flags ---
CC = gcc
# Add standard warning flags, include paths for minishell.h and libft.h
CFLAGS = -Wall -Wextra -Werror -I$(INC_DIR) -I$(LIBFT_PATH)
# Add readline library for linking
LDFLAGS = -lreadline
# Use system 'rm' command
RM = rm -f

# --- Directories ---
SRC_DIR = src/
PARSING_DIR = $(SRC_DIR)parsing/
EXE_DIR = $(SRC_DIR)execution/ # Note: Not currently used in SRCS, but defined
OBJ_DIR = obj/
INC_DIR = inc/
LIBFT_PATH = inc/libft/

# --- Source Files ---
# List all .c files for minishell itself, including paths
SRCS = $(SRC_DIR)main.c \
       $(PARSING_DIR)cmd_cheker.c \
       $(PARSING_DIR)env.c \
       $(PARSING_DIR)parsing.c
# Add execution files here when you create them, e.g.:
#      $(EXE_DIR)executor.c \

# --- Object Files ---
# Create object file names by taking the base name of each source file,
# changing the extension to .o, and prepending the OBJ_DIR.
# Example: src/parsing/cmd_cheker.c -> obj/cmd_cheker.o
OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(patsubst %.c,%.o,$(SRCS)))) # Added trailing slash to OBJ_DIR here

# --- Libft Archive ---
LIBFT_ARCHIVE = $(LIBFT_PATH)libft.a

# --- VPATH Removed ---
# VPATH = $(SRC_DIR):$(PARSING_DIR):$(EXE_DIR) # Removed this line

# --- Main Rules ---

# Default rule: Build the final executable
all: $(NAME)

# Rule to build the libft archive
# This rule runs 'make' within the libft directory
$(LIBFT_ARCHIVE):
	@echo "Making Libft..."
	@$(MAKE) -s -C $(LIBFT_PATH) # Use -s for silent make in libft

# Rule to link the final executable
# Depends on the object files AND the libft archive
$(NAME): $(OBJS) $(LIBFT_ARCHIVE)
	@echo "Linking $(NAME)..."
	@$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBFT_ARCHIVE) $(LDFLAGS) # Link objects, libft, and readline
	@echo "$(NAME) created successfully."

# --- Compilation Rules (Explicit per Directory) ---

# Rule for files directly in SRC_DIR (like main.c)
$(OBJ_DIR)/%.o: $(SRC_DIR)%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling (src) $< -> $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Rule for files in PARSING_DIR
$(OBJ_DIR)/%.o: $(PARSING_DIR)%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling (parsing) $< -> $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Rule for files in EXE_DIR (add sources to SRCS list when ready)
$(OBJ_DIR)/%.o: $(EXE_DIR)%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling (execution) $< -> $@"
	@$(CC) $(CFLAGS) -c -o $@ $<


# --- Cleaning Rules ---

# Remove object files and the object directory
clean:
	@echo "Cleaning object files..."
	@$(RM) -r $(OBJ_DIR)
	@echo "Cleaning Libft..."
	@$(MAKE) -s -C $(LIBFT_PATH) clean

# Remove object files and the final executable
fclean: clean
	@echo "Cleaning $(NAME)..."
	@$(RM) $(NAME)
	@echo "Full cleaning Libft..."
	@$(MAKE) -s -C $(LIBFT_PATH) fclean

# Rebuild everything
re: fclean all

# --- Phony Targets ---
# Declare targets that don't represent actual files
.PHONY: all clean fclean re libft

