
# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -Isqlite
SRCDIR = src
OBJDIR = obj
SQLITEDIR = sqlite
TARGET = atm

# Source files
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/auth.c $(SRCDIR)/system.c $(SRCDIR)/database.c $(SQLITEDIR)/sqlite3.c
OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/auth.o $(OBJDIR)/system.o $(OBJDIR)/database.o $(OBJDIR)/sqlite3.o

# Create obj directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -ldl -lpthread

# Compile source files
$(OBJDIR)/main.o: $(SRCDIR)/main.c src/header.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/main.c -o $@

$(OBJDIR)/auth.o: $(SRCDIR)/auth.c src/header.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/auth.c -o $@

$(OBJDIR)/system.o: $(SRCDIR)/system.c src/header.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/system.c -o $@

$(OBJDIR)/database.o: $(SRCDIR)/database.c src/header.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/database.c -o $@

# Special compilation for sqlite3.c (no warnings, different path)
$(OBJDIR)/sqlite3.o: $(SQLITEDIR)/sqlite3.c
	$(CC) -c $(SQLITEDIR)/sqlite3.c -o $@ -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION

clean:
	rm -rf $(OBJDIR) $(TARGET) atm.db

# Download and setup SQLite amalgamation
download-sqlite:
	@echo "Downloading SQLite amalgamation..."
	@mkdir -p $(SQLITEDIR)
	@if command -v curl >/dev/null 2>&1; then \
		echo "Using curl to download..."; \
		curl -L -o sqlite-amalgamation.zip "https://sqlite.org/2024/sqlite-amalgamation-3450100.zip"; \
	elif command -v wget >/dev/null 2>&1; then \
		echo "Using wget to download..."; \
		wget -O sqlite-amalgamation.zip "https://sqlite.org/2024/sqlite-amalgamation-3450100.zip"; \
	else \
		echo "Error: Neither curl nor wget is available."; \
		echo "Please manually download sqlite-amalgamation-3450100.zip from:"; \
		echo "https://sqlite.org/2024/sqlite-amalgamation-3450100.zip"; \
		echo "Then run: make extract-sqlite"; \
		exit 1; \
	fi
	@echo "Download complete. Extracting..."
	@unzip -q sqlite-amalgamation.zip
	@cp sqlite-amalgamation-3450100/sqlite3.c $(SQLITEDIR)/
	@cp sqlite-amalgamation-3450100/sqlite3.h $(SQLITEDIR)/
	@rm -rf sqlite-amalgamation-3450100 sqlite-amalgamation.zip
	@echo "SQLite setup complete!"

# Extract SQLite if manually downloaded
extract-sqlite:
	@echo "Extracting SQLite amalgamation..."
	@mkdir -p $(SQLITEDIR)
	@if [ -f sqlite-amalgamation.zip ]; then \
		unzip -q sqlite-amalgamation.zip; \
		cp sqlite-amalgamation-3450100/sqlite3.c $(SQLITEDIR)/; \
		cp sqlite-amalgamation-3450100/sqlite3.h $(SQLITEDIR)/; \
		rm -rf sqlite-amalgamation-3450100 sqlite-amalgamation.zip; \
		echo "SQLite extraction complete!"; \
	else \
		echo "Error: sqlite-amalgamation.zip not found"; \
		echo "Please download it manually from https://sqlite.org/download.html"; \
		exit 1; \
	fi

# Check if SQLite files exist
check-sqlite:
	@if [ ! -f $(SQLITEDIR)/sqlite3.c ] || [ ! -f $(SQLITEDIR)/sqlite3.h ]; then \
		echo "Error: SQLite files not found in $(SQLITEDIR)/"; \
		echo "Run 'make download-sqlite' to download automatically"; \
		echo "Or 'make extract-sqlite' if you downloaded manually"; \
		exit 1; \
	fi
	@echo "SQLite files found. Ready to compile."

setup:
	@echo "=== ATM Project Setup ==="
	@echo ""
	@echo "Project Structure:"
	@echo "."
	@echo "├── src/"
	@echo "│   ├── header.h"
	@echo "│   ├── main.c"
	@echo "│   ├── auth.c"
	@echo "│   ├── system.c"
	@echo "│   └── database.c"
	@echo "├── sqlite/"
	@echo "│   ├── sqlite3.c    (downloaded)"
	@echo "│   └── sqlite3.h    (downloaded)"
	@echo "├── obj/             (created automatically)"
	@echo "├── Makefile"
	@echo "└── atm              (executable)"
	@echo ""
	@echo "Setup Commands:"
	@echo "1. make download-sqlite    # Download SQLite automatically"
	@echo "2. make check-sqlite       # Verify SQLite files exist"
	@echo "3. make                    # Compile the project"
	@echo "4. ./atm                   # Run the ATM system"
	@echo ""
	@echo "Alternative (manual download):"
	@echo "1. Download sqlite-amalgamation-XXXXX.zip from sqlite.org"
	@echo "2. Place it in project root as 'sqlite-amalgamation.zip'"
	@echo "3. make extract-sqlite     # Extract to sqlite/ directory"
	@echo "4. make                    # Compile the project"

# Build with dependency checking
all: check-sqlite $(TARGET)

.PHONY: clean download-sqlite extract-sqlite check-sqlite setup all