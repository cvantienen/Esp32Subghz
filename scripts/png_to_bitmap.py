import sys
import logging
from PIL import Image
from pathlib import Path

# Set up logging
logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")
logger = logging.getLogger(__name__)

IMAGE_WIDTH_BY_HEIGHT = [128, 64]

def clean_image_name(file_name:str)-> str:
    removed_period = file_name.replace(".", "").strip()
    removed_dash = removed_period.replace("-", "_").strip()
    cleaned_name = removed_dash
    return cleaned_name

def convert_png_to_bitmap_array(image_path):
    """
    Converts a PNG image to a bitmap array (1 for black, 0 for white).
    """
    try:
        # Open the image using Pillow
        img = Image.open(image_path)
        # Convert to black and white
        img_bnw = img.convert("1")
        # Resize image to desired dimensions
        img_resized = img_bnw.resize(IMAGE_WIDTH_BY_HEIGHT)
        logger.info(f"image size{img_resized.size}")

        # image name cleaned
        image_name = clean_image_name(image_path.stem)

        x11_bitmap_array = img_resized.tobitmap(image_name)
        cpp_bitmap_array = x11_bitmap_array.decode('ascii')
        logger.info(cpp_bitmap_array)
        return cpp_bitmap_array

    except Exception as e:
        logger.error(f"Error processing image {image_path}: {e}")
        return None


def add_comment_to_cpp(bitmap_array, image_name):
    """
    Formats a comment and bitmap array for C++ code.
    """
    comment = f"// Bitmap generated from {image_name}\n"
    try:
        return f"{comment}{bitmap_array}\n\n"
    except Exception as e:
        logger.error(f"Error formatting bitmap array: {e}")
        return None

def start_header_content(directory_name) -> str:
    header_content = f"#ifndef {directory_name}_H\n#define {directory_name}_H\n#include <Arduino.h>\n\n"
    header_content += "// -------------------------------\n"
    header_content += "// Icon Bitmaps\n"
    header_content += (
        "// -----------------------------------------------------------\n\n"
    )
    return header_content


def process_images_in_directory(input_dir, output_header):
    """
    Processes all PNG images in the given directory and converts them to C++ bitmap arrays.
    Saves the result to a header file.
    """
    # Get a list of all PNG files in the input directory
    input_dir = Path(input_dir)
    png_files = list(input_dir.glob("*.png"))

    if not png_files:
        logger.error(f"No PNG files found in the directory: {input_dir}")
        return

    header_content = start_header_content(input_dir.name)
    # Process each PNG file
    for image_path in png_files:
        # image name cleaned
        image_name = clean_image_name(image_path.stem)
        # Convert the PNG to a bitmap array
        bitmap_array = convert_png_to_bitmap_array(image_path)
        if bitmap_array is None:
            continue  # Skip files that failed to convert

        # Format the bitmap array as C++ code
        formatted_array = add_comment_to_cpp(bitmap_array, image_name)
        logger.info(f"formatted_array: {formatted_array}")
        # Add the formatted array to the header content
        header_content += formatted_array

        logger.info(f"Processed: {image_path.name}-- image name used: {image_name}")

    # Finalize the header content
    header_content += "#endif // BITMAPS_H\n"

    # Save the header file
    try:
        with open(output_header, "w") as header_file:
            header_file.write(header_content)
        logger.info(f"Header file saved to: {output_header}")
    except Exception as e:
        logger.error(f"Error writing to file {output_header}: {e}")


def main():
    # Input directory (where the PNG images are stored)
    current_dir = Path(__file__).parent
    png_dir = current_dir / "animation_png"
    for animation_dir in png_dir.iterdir():
        if animation_dir.is_dir():
            output_header_path = current_dir / (animation_dir.name + "_bitmaps.h")
            process_images_in_directory(animation_dir, output_header_path)
    logger.info("Complete")


if __name__ == "__main__":
    sys.exit(main())
