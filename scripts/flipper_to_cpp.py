import re
from pathlib import Path
from dataclasses import dataclass
from typing import Optional
import logging
from collections import defaultdict

# logging configuration
logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")
logger = logging.getLogger(__name__)

# set current directory to Base directory
BASE_DIR = Path.cwd().parent
print("--------------------------------")
print(BASE_DIR)
print("--------------------------------")
# ============================================================================
# CONFIGURATION
# ============================================================================

FLIPPER_SIGNALS_DIR = BASE_DIR / "data/subghz"
OUTPUT_HEADER = BASE_DIR / "generated_signals.h"
OUTPUT_SOURCE = BASE_DIR / "generated_signals.cpp"

testfile = FLIPPER_SIGNALS_DIR / "TouchTunesPin/0.sub"
if not testfile.exists():
    print(f"File {testfile} does not exist")
    exit(1)
# ============================================================================
# DATA STRUCTURES
# ============================================================================


@dataclass
class FlipperSignal:
    """Represents a parsed Flipper .sub file"""

    name: str
    filename: str
    category: str
    frequency: float  # MHz
    raw_data: list[int]
    description: str = ""
    protocol: str = "RAW"
    preset: str = ""


def parse_flipper_sub_file(
    filepath: Path, category: str = "MISC"
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

    # RAW_Data can span multiple generated_source_code
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

    # Parse all RAW_Data generated_source_code and combine
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
        desc_match.group(1).strip(
        ) if desc_match else f"Signal from {filepath.name}"
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


def scan_flipper_directory(flipper_signals_dir: Path) -> dict[str, list[FlipperSignal]]:
    categories: dict[str, list[FlipperSignal]] = defaultdict(list)

    for category_dir in flipper_signals_dir.iterdir():
        if not category_dir.is_dir() or category_dir.name.startswith("."):
            continue

        logger.info("Scanning category: %s", category_dir.name)

        for sub_file in category_dir.glob("*.sub"):
            signal = parse_flipper_sub_file(sub_file, category_dir.name)
            if signal:
                categories[category_dir.name].append(signal)

    return categories


# ============================================================================
# C++ CODE GENERATOR
# ============================================================================
def cat_macro(name: str) -> str:
    return sanitize_identifier(name).upper()


def sample_name(cat: str, sig: str) -> str:
    return f"samples_{sanitize_identifier(cat)}_{sanitize_identifier(sig)}"


def length_name(cat: str, sig: str) -> str:
    return (
        f"LENGTH_SAMPLES_{sanitize_identifier(cat)}_{sanitize_identifier(sig)}".upper(
        )
    )


def signal_array_name(cat: str) -> str:
    return f"{cat_macro(cat)}_SIGNALS"


def num_name(cat: str) -> str:
    return f"NUM_{cat_macro(cat)}"


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


def generate_code(categories: dict[str, list[FlipperSignal]]):
    header = []
    source = []

    # ================= HEADER =================

    header.extend(
        [
            "#ifndef GENERATED_SIGNALS_H",
            "#define GENERATED_SIGNALS_H",
            "",
            "#include <Arduino.h>",
            "#include <pgmspace.h>",
            "",
            "// ==================== ARRAY LENGTH CONSTANTS ====================",
            "",
        ]
    )

    for cat, signals in categories.items():
        for s in signals:
            header.append(
                f"constexpr uint16_t {length_name(cat, s.name)} = {len(s.raw_data)};"
            )

    header.extend(
        [
            "",
            "// ==================== EXTERN DECLARATIONS ====================",
            "",
        ]
    )

    # Sample arrays
    for cat, signals in categories.items():
        for s in signals:
            header.append(
                f"extern const int16_t {sample_name(cat, s.name)}[] PROGMEM;")

    header.append("")

    # Signal arrays + counts
    for cat in categories:
        header.append(f"extern SubGHzSignal {signal_array_name(cat)}[];")
        header.append(f"extern const uint8_t {num_name(cat)};")

    header.extend(
        [
            "",
            "extern SubghzSignalList SIGNAL_CATEGORIES[];",
            "extern const uint8_t NUM_OF_CATEGORIES;",
            "",
            "#endif",
        ]
    )

    # ================= SOURCE =================

    source.extend(
        [
            '#include "signals.h"',
            '#include "generated_signals.h"',
            "",
            "// ==================== SAMPLE DATA ARRAYS ====================",
            "",
        ]
    )

    for cat, signals in categories.items():
        for s in signals:
            values = []
            for i in range(0, len(s.raw_data), 8):
                values.append(
                    "    " + ", ".join(map(str, s.raw_data[i: i + 8])))

            source.append(
                f"const int16_t {sample_name(cat, s.name)}[] PROGMEM = {{")
            source.append(",\n".join(values))
            source.append("};\n")

    # Signal arrays
    for cat, signals in categories.items():
        source.append(f"SubGHzSignal {signal_array_name(cat)}[] = {{")

        for i, s in enumerate(signals):
            comma = "," if i < len(signals) - 1 else ""
            source.append(
                f'    {{"{s.name}", "{s.description}", '
                f"{sample_name(cat, s.name)}, "
                f"{length_name(cat, s.name)}, {s.frequency:.2f}}}{comma}"
            )

        source.append("};")
        source.append(
            f"const uint8_t {num_name(cat)} = sizeof({signal_array_name(cat)}) / sizeof(SubGHzSignal);\n"
        )

    # Categories
    source.append("SubghzSignalList SIGNAL_CATEGORIES[] = {")

    for i, cat in enumerate(categories):
        comma = "," if i < len(categories) - 1 else ""
        source.append(
            f'    {{"{cat}", {signal_array_name(cat)}, {num_name(cat)}}}{comma}'
        )

    source.extend(
        [
            "};",
            "",
            "const uint8_t NUM_OF_CATEGORIES = sizeof(SIGNAL_CATEGORIES) / sizeof(SubghzSignalList);",
        ]
    )

    return "\n".join(source), "\n".join(header)


def main():
    # Scan for .sub files
    categories = scan_flipper_directory(FLIPPER_SIGNALS_DIR)

    if not categories:
        logger.warning("No categories found. Nothing to generate.")
        return 1

    # Generate C++ code
    generated_source_output, generated_header_output = generate_code(
        categories)

    # Write output files
    header_path = BASE_DIR / OUTPUT_HEADER
    source_path = BASE_DIR / OUTPUT_SOURCE

    try:
        header_path.write_text(generated_header_output, encoding="utf-8")
        logger.info("Generated: %s", header_path)
    except Exception as e:
        logger.error("Failed to write header file %s: %s", header_path, e)
        return

    try:
        source_path.write_text(generated_source_output, encoding="utf-8")
        logger.info("Generated: %s", source_path)
    except Exception as e:
        logger.error("Failed to write source file %s: %s", source_path, e)
        return

    logger.info("C++ code generation complete.")
    return


if __name__ == "__main__":
    main()
