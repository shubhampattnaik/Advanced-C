#ifndef DECODE_H
#define DECODE_H

#include "common.h"

#include "types.h" // Contains user defined types

/* Structure to store information required for
 * decoding secret file from Stego Image
 * Info about output and intermediate data is
 * also stored
 */


/* Decoding function prototype */


/* Get File pointers for i/p and o/p files */
Status open_files_decode(EncodeInfo *encInfo);			

/* Create decode.txt */
Status create_files_decode(EncodeInfo *encInfo);			

/* Close decode files */
void close_files_decode(EncodeInfo *encInfo);

/* Extract Magic String */				
Status decode_magic_string(FILE *fptr_stego_image);

/* Extract file size */
uint decode_secret_file_size(FILE * fptr_stego_image);

/* Encode secret file extenstion */
Status decode_secret_file_extn(FILE *fptr_stego_image);

/* Decode secret file data*/				//done
Status decode_secret_file_data(FILE *fptr_secret, FILE *fptr_stego_image, long size_secret_file);


/* Decode a byte from LSB of image data to an array */
Status decode_byte_from_lsb(char *data, char *image_data);

/* Copy remaining image bytes from src to stego image after encoding */		//done
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
