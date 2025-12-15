import sys
import logging
from PIL import Image, ImageOps
from pathlib import Path

# Set up logging
logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")
logger = logging.getLogger(__name__)

IMAGE_WIDTH_BY_HEIGHT = [128, 64]


def clean_image_name(file_name: str) -> str:
    removed_period = file_name.replace(".", "").strip()
    removed_dash = removed_period.replace("-", "_").strip()
    cleaned_name = removed_dash
    return cleaned_name


def image_to_u8g2_bitmap(path, invert=False):
    # Load image
    try:
        img = Image.open(path).convert("1")  # convert to B/W (1-bit)
        img_resized = img.resize(IMAGE_WIDTH_BY_HEIGHT)
        logger.info(f"image size{img_resized.size}")
        width, height = img_resized.size
        pixels = img_resized.load()

        u8g2_bitmap = []

        # U8g2 expects vertical slices (8 pixels tall per byte)
        for x in range(width):
            for y in range(0, height, 8):
                byte = 0
                for bit in range(8):
                    if y + bit < height:
                        pixel_on = pixels[x, y + bit] == 0  # black pixel
                        if invert:
                            pixel_on = not pixel_on
                        if pixel_on:
                            byte |= 1 << bit
                u8g2_bitmap.append(byte)

        return u8g2_bitmap, width, height
    except Exception as e:
        logger.error(f"Error processing image {path}: {e}")
        return None


def image_resize(path):
    # Load image
    try:
        # Ensure RGB
        img = Image.open(path).convert("RGB")
        # Invert colors
        img_resized = img.resize(IMAGE_WIDTH_BY_HEIGHT)
        img_resized.save(path)
        logger.info(f"Processed and saved image: {path}")
    except Exception as e:
        logger.error(f"Error processing image {path}: {e}")
        return None
    

def bitmap_to_cpp(data, variable_name="my_bitmap", width=None, height=None):
    hexbytes = ", ".join(f"0x{b:02X}" for b in data)

    if width is None or height is None:
        c_array = f"const uint8_t {variable_name}[] PROGMEM = {{\n  {hexbytes}\n}};\n\n"
        return c_array
    else:
        c_array = (
            f"#define {variable_name}_{width}\n"
            f"#define {variable_name}_{height}\n"
            f"const uint8_t {variable_name}[] PROGMEM = {{\n"
            f"  {hexbytes}\n"
            f"}};\n\n"
        )

        return c_array


# # ---------- Example Usage -------------
# input_image = "input.png"
# data, w, h = image_to_u8g2_bitmap(input_image)

# print("// Width:", w, "Height:", h)
# print(to_c_array(data, "my_bitmap"))


def save_header_file(directory_name, output_header_file, headerfile_content):
    header_content = f"#ifndef {directory_name}_H\n#define {directory_name}_H\n#include <Arduino.h>\n\n"
    header_content += "// -------------------------------\n"
    header_content += "// Icon Bitmaps\n"
    header_content += (
        "// -----------------------------------------------------------\n\n"
    )
    header_content += headerfile_content
    # Finalize the header content
    header_content += f"#endif // {directory_name}_H\n"
    # Save the header file
    try:
        with open(output_header_file, "w") as header_file:
            header_file.write(header_content)

        logger.info(f"Header file saved to: {output_header_file}")
        logger.info(f"image directory:\n{directory_name}\n")
    except Exception as e:
        logger.error(f"Error writing to file {output_header_file}: {e}")


def process_images_in_directory(input_dir):
    """
    Processes all PNG images in the given directory
    and converts them to C++ bitmap arrays.
    """
    # Get a list of all PNG files in the input directory
    input_dir = Path(input_dir)
    png_files = list(input_dir.glob("*.png"))

    if not png_files:
        logger.error(f"No PNG files found in the directory: {input_dir}")
        return
    header_content = ""
    for image_path in png_files:
        # Resize and invert the image
        image_resize(image_path)
        # # image name cleaned
        # image_name = clean_image_name(image_path.stem)
        # # Convert the PNG to a bitmap array
        # bitmap_array, width, height = image_to_u8g2_bitmap(image_path)
        # if bitmap_array is None:
        #     continue

        # # Add comments about the image
        # header_content += f"// Bitmap for {image_path.name}\n"
        # # Format the bitmap array as C++ code
        # cpp_array = bitmap_to_cpp(bitmap_array, image_name, width, height)
        # logger.info(f"cpp_array: {cpp_array}")
        # # Add the formatted array to the header content
        # header_content += cpp_array

        # logger.info(f"Processed: {image_path.name}-- image name used: {image_name}")

    return header_content


def main():
    # Input directory (where the PNG images are stored)
    current_dir = Path(__file__).parent
    png_dir = current_dir / "animation_png"
    for animation_dir in png_dir.iterdir():
        if animation_dir.is_dir():
            process_images_in_directory(animation_dir)
            # output_header_path = current_dir / (animation_dir.name + "_bitmaps.h")
            #bitmap_arrays = process_images_in_directory(animation_dir)
            # if bitmap_arrays:
            #     save_header_file(
            #         animation_dir.name.upper(), output_header_path, bitmap_arrays
            #     )
    logger.info("Complete")


if __name__ == "__main__":
    sys.exit(main())
