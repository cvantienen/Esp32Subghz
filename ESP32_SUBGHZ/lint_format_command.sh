#!/bin/bash
# lint_and_format.sh - Code formatting and analysis script for ESP32 PlatformIO project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$PROJECT_DIR/src"
INCLUDE_DIR="$PROJECT_DIR/include"
SCRIPTS_DIR="$PROJECT_DIR/scripts"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  ESP32 SubGHz Code Quality Tools${NC}"
echo -e "${BLUE}========================================${NC}"

# Find all C/C++ source files
CPP_FILES=$(find "$SRC_DIR" "$INCLUDE_DIR" -type f \( -name "*.cpp" -o -name "*.c" -o -name "*.h" -o -name "*.hpp" \) 2>/dev/null)

# Find all Python files (exclude venv)
PY_FILES=$(find "$SCRIPTS_DIR" -type f -name "*.py" -not -path "*/venv/*" 2>/dev/null)

# ============================================================================
# C/C++ SECTION
# ============================================================================
echo -e "\n${BLUE}--- C/C++ Code Analysis ---${NC}"

# 1. CLANG-FORMAT
echo -e "\n${YELLOW}[1/4] Running clang-format...${NC}"
if command -v clang-format &> /dev/null; then
    for file in $CPP_FILES; do
        echo "  Formatting: $file"
        clang-format -i "$file"
    done
    echo -e "${GREEN}✓ clang-format complete${NC}"
else
    echo -e "${RED}✗ clang-format not found. Install with: sudo apt install clang-format${NC}"
fi

# 2. CLANG-TIDY
echo -e "\n${YELLOW}[2/4] Running clang-tidy...${NC}"
if command -v clang-tidy &> /dev/null; then
    if [ -f "$PROJECT_DIR/compile_commands.json" ]; then
        for file in $CPP_FILES; do
            echo "  Analyzing: $file"
            clang-tidy "$file" -p "$PROJECT_DIR" --quiet 2>/dev/null || true
        done
        echo -e "${GREEN}✓ clang-tidy complete${NC}"
    else
        echo -e "${RED}✗ compile_commands.json not found. Run 'pio run' first.${NC}"
    fi
else
    echo -e "${RED}✗ clang-tidy not found. Install with: sudo apt install clang-tidy${NC}"
fi

# 3. CPPCHECK
echo -e "\n${YELLOW}[3/4] Running cppcheck...${NC}"
if command -v cppcheck &> /dev/null; then
    cppcheck --enable=all --suppress=missingIncludeSystem \
             --inline-suppr --quiet \
             -I "$INCLUDE_DIR" \
             "$SRC_DIR" "$INCLUDE_DIR" 2>&1
    echo -e "${GREEN}✓ cppcheck complete${NC}"
else
    echo -e "${RED}✗ cppcheck not found. Install with: sudo apt install cppcheck${NC}"
fi

# 4. PLATFORMIO CHECK
echo -e "\n${YELLOW}[4/4] Running PlatformIO check...${NC}"
if command -v pio &> /dev/null; then
    cd "$PROJECT_DIR"
    pio check --skip-packages --severity=medium 2>&1 || true
    echo -e "${GREEN}✓ PlatformIO check complete${NC}"
else
    echo -e "${RED}✗ PlatformIO not found. Install with: pip install platformio${NC}"
fi

# ============================================================================
# PYTHON SECTION
# ============================================================================
echo -e "\n${BLUE}--- Python Code Analysis ---${NC}"

# 5. BLACK (Code Formatter)
echo -e "\n${YELLOW}[5/8] Running black (formatter)...${NC}"
if command -v black &> /dev/null; then
    black --line-length 100 $PY_FILES 2>&1
    echo -e "${GREEN}✓ black complete${NC}"
else
    echo -e "${RED}✗ black not found. Install with: pip install black${NC}"
fi

# 6. ISORT (Import Sorter)
echo -e "\n${YELLOW}[6/8] Running isort (import sorter)...${NC}"
if command -v isort &> /dev/null; then
    isort --profile black --line-length 100 $PY_FILES 2>&1
    echo -e "${GREEN}✓ isort complete${NC}"
else
    echo -e "${RED}✗ isort not found. Install with: pip install isort${NC}"
fi

# 7. RUFF (Fast Linter)
echo -e "\n${YELLOW}[7/8] Running ruff (linter)...${NC}"
if command -v ruff &> /dev/null; then
    ruff check --fix $PY_FILES 2>&1 || true
    echo -e "${GREEN}✓ ruff complete${NC}"
else
    echo -e "${RED}✗ ruff not found. Install with: pip install ruff${NC}"
fi

# 8. MYPY (Type Checker)
echo -e "\n${YELLOW}[8/8] Running mypy (type checker)...${NC}"
if command -v mypy &> /dev/null; then
    mypy --ignore-missing-imports $PY_FILES 2>&1 || true
    echo -e "${GREEN}✓ mypy complete${NC}"
else
    echo -e "${RED}✗ mypy not found. Install with: pip install mypy${NC}"
fi

# ============================================================================
# SUMMARY
# ============================================================================
echo -e "\n${BLUE}========================================${NC}"
echo -e "${GREEN}  Analysis Complete!${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "  C/C++ files: $(echo $CPP_FILES | wc -w)"
echo -e "  Python files: $(echo $PY_FILES | wc -w)"
echo -e "${BLUE}========================================${NC}"