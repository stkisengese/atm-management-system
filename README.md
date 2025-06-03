# ATM Banking System

## Overview
The ATM Banking System is a secure, feature-rich command-line banking application with enhanced UI elements and robust functionality. Built with C and SQLite3, it provides a complete banking experience with modern terminal-based interface elements.

## Key Improvements

### Enhanced User Interface
- **Box-style menus** with clear visual boundaries
- **Improved formatting** for better readability
- **Consistent styling** across all screens
- **Welcome messages** personalized with username

### New Features
- **Logout functionality** (switch users without exiting)
- **Auto-login after registration**
- **Better error handling** with clear messages
- **Input validation** for all user entries

### Technical Improvements
- **Makefile support** for easy compilation
- **SQLite amalgamation** included
- **Modular code structure** for maintainability
- **Memory-safe operations**

## Installation Guide

### Prerequisites
- GCC compiler (version 9.0 or higher recommended)
- SQLite3 development libraries
- Linux/Unix environment (for termios)
- make utility

### Build Process
1. Clone the repository
2. Set up SQLite (automatic or manual):
   ```bash
   make download-sqlite  # Automatic download
   # OR
   make extract-sqlite   # For manual download
   ```
3. Compile the project:
   ```bash
   make
   ```
4. Run the executable:
   ```bash
   ./atm
   ```

## Complete Feature Set

### Authentication System
- **Login**: Secure credential entry with password masking
- **Registration**: New user creation with validation
- **Password Requirements**: 4-12 characters (alphanumeric with -/_)
- **Auto-login**: Immediate access after registration

### Account Management
- **Create Accounts**: 
  - Savings, Current, and Fixed Deposit options
  - Detailed account type descriptions
- **View Accounts**: 
  - Individual account details
  - Complete portfolio overview
- **Update Information**: 
  - Country and phone number modifications
- **Delete Accounts**: 
  - Multi-step confirmation process
  - Clear warnings about permanent deletion
- **Transfer Ownership**: 
  - Secure transfer to other users
  - Complete audit trail

### Transaction Processing
- **Deposits**: Add funds with validation
- **Withdrawals**: Remove funds with balance checks
- **Transaction History**: View past activities

### Account Types
1. **Savings Account**
   - Interest-earning
   - Flexible transactions
   - Long-term savings focus

2. **Current Account**
   - No interest
   - Unlimited transactions
   - Business/personal use

3. **Fixed Deposit Accounts**
   - Term options: 1, 2, or 3 years
   - Higher interest rates
   - Funds locked until maturity
   - No withdrawals during term

## Usage Instructions

### System Flow
1. **Launch Application**: Run `./atm` from terminal
2. **Authentication**:
   - Existing users: Login with credentials
   - New users: Complete registration
3. **Main Menu**: Access all banking functions
4. **Operations**: Select desired banking action
5. **Logout/Exit**: End session or switch users

### Menu Options
```
╔═══════════════════════════════════════════╗
║  [1] - Create a new account               ║
║  [2] - Update account information         ║
║  [3] - Check accounts                     ║
║  [4] - Check list of owned account        ║
║  [5] - Make Transaction                   ║
║  [6] - Remove existing account            ║
║  [7] - Transfer ownership                 ║
║  [8] - Logout (Switch User)               ║
║  [9] - Exit System                        ║
╚═══════════════════════════════════════════╝
```

## Technical Documentation

### Database Schema
- **users** table:
  - id (INTEGER PRIMARY KEY)
  - name (TEXT UNIQUE NOT NULL)
  - password (TEXT NOT NULL)

- **records** table:
  - id (INTEGER PRIMARY KEY)
  - user_id (INTEGER NOT NULL)
  - user_name (TEXT NOT NULL)
  - account_id (INTEGER NOT NULL)
  - deposit_date (TEXT NOT NULL)
  - country (TEXT NOT NULL)
  - phone (TEXT NOT NULL)
  - balance (REAL NOT NULL)
  - account_type (TEXT NOT NULL)

### Input Validation System
- **Names**: 2-26 chars, letters/spaces/hyphens only
- **Passwords**: 4-12 chars, alphanumeric with -/_
- **Dates**: Valid calendar dates (1900-2100)
- **Phone Numbers**: International format support
- **Account Numbers**: 6-12 digits, no leading zeros
- **Amounts**: Positive numbers with 2 decimal places

### Security Features
- Password masking during input
- SQL parameterized queries
- Account ownership verification
- Sensitive operation confirmations
- Input sanitization

## Build System

### Makefile Targets
- **make**: Compile the application
- **make clean**: Remove build artifacts
- **make download-sqlite**: Auto-download SQLite
- **make extract-sqlite**: Manual SQLite setup
- **make check-sqlite**: Verify SQLite files
- **make setup**: Show project structure

### Compilation Flags
- `-Wall -Wextra`: Maximum warnings
- `-std=c99`: C language standard
- `-DSQLITE_THREADSAFE=0`: Single-threaded mode
- `-DSQLITE_OMIT_LOAD_EXTENSION`: Security hardening

## Troubleshooting

### Common Issues
1. **SQLite Errors**:
   - Run `make download-sqlite` to ensure proper setup
   - Verify file permissions in project directory

2. **Compilation Problems**:
   - Check GCC version (`gcc --version`)
   - Install build-essential package if missing

3. **Input Issues**:
   - Follow on-screen format requirements exactly
   - Use correct input types (numbers vs text)

4. **Terminal Display**:
   - Ensure terminal supports UTF-8 characters
   - Use standard terminal emulators (not IDE terminals)

## License
This project is open-source software licensed under the MIT License.

## Author(s)
Developed by [Stephen Kisengese](https://github.com/stkisengese)

## Support
For additional assistance, please consult:
- Project documentation
- SQLite official documentation
- GNU Make manual
- C programming references
