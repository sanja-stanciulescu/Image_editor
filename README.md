# Photoshop who?

Using the functionalities of Adobe Photoshop as reference, the program is a text editor that supports a set of operations over PPm or PGM-formatted images.
The main focus of this project lies in the way heap memory is handled, as well as using complex mathematical operations in a more practical context.\n
The functionalities are as follows:

* LOAD: Frees memory of the previous image, opens a new file, allocates memory based on image type (grayscale or color), and reads the data from the file.

* SELECT: Reads parameters to determine the selection, either "ALL" or specific coordinates, and checks their validity.

* SELECT ALL: Selects the entire image.

* CROP: Creates a new structure to store cropped image data, copies the selected pixels, swaps memory zones, and frees old memory.

* SAVE: Saves the image to a new file in either ASCII or binary format, copying the data from the matrix based on the format.

* APPLY: Manages error handling and applies filters (EDGE, BLUR, SHARPEN, GAUSSIAN_BLUR) by generating kernels, processing pixels, and replacing old data with the new.

* EQUALIZE: Creates a frequency histogram for grayscale values and equalizes the image, updating pixel values.

* HISTOGRAM: Generates a histogram of grayscale values, creates a star count vector, and displays it.

* ROTATE: Rotates the image or selected portion by 90°, -90°, or 180°, depending on the command, and creates new memory zones for the rotated data.

* EXIT: Frees image memory and exits the program.

* The main function processes commands and handles errors, calling specific functions based on the input.
