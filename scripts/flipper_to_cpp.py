
# ============================================================================
# FLIPPER FILE PARSER
# ============================================================================

def parse_flipper_sub_file(
    filepath: Path, category: str = MISC_CATEGORY
) -> Optional[FlipperSignal]:
    """
    Parse a Flipper Zero .sub file and extract signal data.

    Flipper .sub file format:
        Filetype: Flipper SubGhz RAW File
        Version: 1
        Frequency: 433920000
        Preset: FuriHalSubGhzPresetOok650Async
        Protocol: RAW
        RAW_Data: 500 -1000 500 -500 ...
    """
    try:
        content = filepath.read_text(encoding="utf-8")
    except FileNotFoundError:
        logger.error("Could not read %s: File not found", filepath)
        return None
    except (OSError, UnicodeDecodeError) as e:
        logger.error("Could not read %s: %s", filepath, e)
        return None

    # Extract fields using regex
    frequency_match = re.search(r"Frequency:\s*(\d+)", content)
    protocol_match = re.search(r"Protocol:\s*(\w+)", content)
    preset_match = re.search(r"Preset:\s*(\S+)", content)

    # RAW_Data can span multiple lines
    raw_data_matches = re.findall(r"RAW_Data:\s*([-\d\s]+)", content)

    if not frequency_match:
        logger.warning("No frequency found in %s, skipping", filepath.name)
        return None

    if not raw_data_matches:
        logger.warning("No RAW_Data found in %s, skipping", filepath.name)
        return None

    # Parse frequency (Flipper uses Hz, we want MHz)
    frequency_hz = int(frequency_match.group(1))
    frequency_mhz = frequency_hz / 1_000_000.0

    # Parse all RAW_Data lines and combine
    raw_data = []
    for raw_line in raw_data_matches:
        values = raw_line.strip().split()
        for val in values:
            try:
                raw_data.append(int(val))
            except ValueError:
                continue

    if not raw_data:
        logger.warning("Empty RAW_Data in %s, skipping", filepath.name)
        return None

    # Generate name from filename
    name = filepath.stem.replace("_", " ").replace("-", " ").title()

    # Try to extract description from file comments or use filename
    desc_match = re.search(r"#\s*Description:\s*(.+)", content, re.IGNORECASE)
    description = (
        desc_match.group(1).strip()
        if desc_match
        else f"Signal from {filepath.name}"
    )

    return FlipperSignal(
        name=name,
        filename=filepath.stem,
        category=category,
        frequency=frequency_mhz,
        raw_data=raw_data,
        description=description,
        protocol=protocol_match.group(1) if protocol_match else "RAW",
        preset=preset_match.group(1) if preset_match else "",
    )


# ============================================================================
# DIRECTORY SCANNER
# ============================================================================

def scan_flipper_directory(input_dir: Path) -> list[SignalCategory]:
    """
    Scan a directory for .sub files organized into category folders.

    Returns a list of SignalCategory objects.
    """
    categories: dict[str, list[FlipperSignal]] = {}

    # First pass: scan subdirectories (categories)
    for item in input_dir.iterdir():
        if item.is_dir() and not item.name.startswith("."):
            category_name = item.name.replace("_", " ").replace("-", " ")
            logger.info("Category: %s", category_name)

            if category_name not in categories:
                categories[category_name] = []

            for sub_file in item.glob("*.sub"):
                logger.debug("  → Parsing: %s", sub_file.name)
                signal = parse_flipper_sub_file(sub_file, category_name)
                if signal:
                    categories[category_name].append(signal)

    # Second pass: scan root-level .sub files (go to Misc category)
    root_subs = list(input_dir.glob("*.sub"))
    if root_subs:
        logger.info("Category: %s (root-level files)", MISC_CATEGORY)
        if MISC_CATEGORY not in categories:
            categories[MISC_CATEGORY] = []

        for sub_file in root_subs:
            logger.debug("  → Parsing: %s", sub_file.name)
            signal = parse_flipper_sub_file(sub_file, MISC_CATEGORY)
            if signal:
                categories[MISC_CATEGORY].append(signal)

    # Convert to list of SignalCategory objects
    result = []
    for cat_name, signals in sorted(categories.items()):
        if signals:  # Only include categories with signals
            result.append(SignalCategory(name=cat_name, signals=signals))

    return result


# ============================================================================
# C++ CODE GENERATOR
# ============================================================================


def sanitize_identifier(name: str) -> str:
    """Convert a name to a valid C++ identifier"""
    # Replace spaces and special chars with underscores
    identifier = re.sub(r"[^a-zA-Z0-9]", "_", name)
    # Remove consecutive underscores
    identifier = re.sub(r"_+", "_", identifier)
    # Remove leading/trailing underscores
    identifier = identifier.strip("_")
    # Ensure it doesn't start with a number
    if identifier and identifier[0].isdigit():
        identifier = "sig_" + identifier
    return identifier.lower()


def generate_header(categories: list[SignalCategory]) -> str:
    """Generate the C++ header file content"""
    lines = [
        "// ============================================================================",
        "// AUTO-GENERATED FILE - DO NOT EDIT MANUALLY",
        "// Generated by flipper_to_cpp.py from Flipper Zero .sub files",
        "",
        "#ifndef GENERATED_SIGNALS_H",
        "#define GENERATED_SIGNALS_H",
        "",
        "#include <Arduino.h>",
        "#include <pgmspace.h>",
        '#include "signals.h"  // For SubGHzSignal and SubghzSignalList structs',
        "",
        "// ==================== ARRAY LENGTH CONSTANTS ====================",
        "",
    ]

    # Generate length constants for each signal
    for category in categories:
        for signal in category.signals:
            const_name = (
                f"LENGTH_GEN_{sanitize_identifier(category.name)}_"
                f"{sanitize_identifier(signal.filename)}"
            ).upper()
            lines.append(
                f"constexpr uint16_t {const_name} = {len(signal.raw_data)};"
            )

    lines.extend(
        [
            "",
            "// ==================== EXTERN DECLARATIONS ====================",
            "",
            "// Sample arrays (stored in PROGMEM/flash)",
        ]
    )

    # Declare sample arrays
    for category in categories:
        for signal in category.signals:
            var_name = (
                f"gen_samples_{sanitize_identifier(category.name)}_"
                f"{sanitize_identifier(signal.filename)}"
            )
            lines.append(f"extern const int16_t {var_name}[] PROGMEM;")

    lines.extend(["", "// Signal arrays"])

    # Declare signal arrays per category
    for category in categories:
        array_name = f"GEN_{sanitize_identifier(category.name).upper()}_SIGNALS"
        lines.append(f"extern SubGHzSignal {array_name}[];")

    lines.extend(["", "// Counts"])

    for category in categories:
        count_name = f"NUM_GEN_{sanitize_identifier(category.name).upper()}"
        lines.append(f"extern const uint8_t {count_name};")

    lines.extend(
        [
            "",
            "// Generated Categories List",
            "extern SubghzSignalList GEN_SIGNAL_CATEGORIES[];",
            "extern const uint8_t NUM_GEN_CATEGORIES;",
            "",
            "#endif  // GENERATED_SIGNALS_H",
            "",
        ]
    )

    return "\n".join(lines)


def generate_source(categories: list[SignalCategory]) -> str:
    """Generate the C++ source file content"""
    lines = [
        "// ============================================================================",
        "// AUTO-GENERATED FILE - DO NOT EDIT MANUALLY",
        "// Generated by flipper_to_cpp.py from Flipper Zero .sub files",
        "",
        '#include "generated_signals.h"',
        "",
        "// ==================== SAMPLE DATA ARRAYS (STORED IN FLASH) ====================",
        "// PROGMEM stores these in flash memory instead of RAM",
        "// ESP32 reads PROGMEM automatically - no special read functions needed",
        "",
    ]

    # Generate sample arrays
    for category in categories:
        lines.append(f"// --- {category.name} ---")
        for signal in category.signals:
            var_name = (
                f"gen_samples_{sanitize_identifier(category.name)}_"
                f"{sanitize_identifier(signal.filename)}"
            )

            # Format raw data nicely (max ~8 values per line)
            data_lines = []
            for i in range(0, len(signal.raw_data), 8):
                chunk = signal.raw_data[i : i + 8]
                data_lines.append("    " + ", ".join(str(v) for v in chunk))

            lines.append(f"const int16_t {var_name}[] PROGMEM = {{")
            lines.append(",\n".join(data_lines))
            lines.append("};")
            lines.append("")

    lines.extend(
        [
            "// ==================== SIGNAL MENU STRUCTURES ====================",
            "",
        ]
    )

    # Generate signal arrays per category
    for category in categories:
        array_name = f"GEN_{sanitize_identifier(category.name).upper()}_SIGNALS"
        lines.append(f"SubGHzSignal {array_name}[] = {{")

        for i, signal in enumerate(category.signals):
            var_name = (
                f"gen_samples_{sanitize_identifier(category.name)}_"
                f"{sanitize_identifier(signal.filename)}"
            )
            const_name = (
                f"LENGTH_GEN_{sanitize_identifier(category.name)}_"
                f"{sanitize_identifier(signal.filename)}"Path

            # Escape quotes in name/description
            safe_name = signal.name.replace('"', '\\"')
            safe_desc = signal.description.replace('"', '\\"')

            comma = "," if i < len(category.signals) - 1 else ""
            lines.append(
                f'    {{"{safe_name}", "{safe_desc}", {var_name}, '
                f'{const_name}, {signal.frequency:.2f}f}}{comma}'
            )

        lines.append("};")
        count_name = f"NUM_GEN_{sanitize_identifier(category.name).upper()}"
        lines.append(
            f"const uint8_t {count_name} = "
            f"sizeof({array_name}) / sizeof(SubGHzSignal);"
        )
        lines.append("")

    lines.extend(
        [
            "// ==================== GENERATED CATEGORIES ====================",
            "",
            "SubghzSignalList GEN_SIGNAL_CATEGORIES[] = {",
        ]
    )

    # Generate categories list
    for i, category in enumerate(categories):
        array_name = f"GEN_{sanitize_identifier(category.name).upper()}_SIGNALS"
        count_name = f"NUM_GEN_{sanitize_identifier(category.name).upper()}"
        safe_name = category.name.replace('"', '\\"')
        comma = "," if i < len(categories) - 1 else ""
        lines.append(
            f'    {{"{safe_name}", {array_name}, {count_name}}}{comma}'
        )

    lines.extend(
        [
            "};",
            "",
            "const uint8_t NUM_GEN_CATEGORIES = "
            "sizeof(GEN_SIGNAL_CATEGORIES) / sizeof(SubghzSignalList);",
            "",
        ]
    )

    return "\n".join(lines)

# ============================================================================
# MAIN ENTRY POINT
# ============================================================================


def main() -> int:
    """Main entry point for the script."""
    # Determine project root (parent of scripts/)
    script_dir = Path(__file__).parent.resolve()
    project_root = script_dir.parent

    # Determine input directory
    if len(sys.argv) > 1:
        input_dir = Path(sys.argv[1])
        if not input_dir.is_absolute():
            input_dir = project_root / input_dir
    else:
        input_dir = project_root / DEFAULT_INPUT_DIR

    # Scan for .sub files
    categories = scan_flipper_directory(input_dir)

    if not categories:
        logger.warning("No categories found. Nothing to generate.")
        return 1

    # Generate C++ code
    header_content = generate_header(categories)
    source_content = generate_source(categories)

    # Write output files
    header_path = project_root / OUTPUT_HEADER
    source_path = project_root / OUTPUT_SOURCE

    # Ensure output directories exist
    header_path.parent.mkdir(parents=True, exist_ok=True)
    source_path.parent.mkdir(parents=True, exist_ok=True)

    try:
        header_path.write_text(header_content, encoding="utf-8")Path
        return 1

    try:
        source_path.write_text(source_content, encoding="utf-8")
        logger.info("Generated: %s", source_path)
    except OSError as e:
        logger.error("Failed to write source file %s: %s", source_path, e)
        return 1

    logger.info("C++ code generation complete.")
    return 0


if __name__ == "__main__":
    sys.exit(main())