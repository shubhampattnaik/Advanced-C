#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"


/* Function Definitions */

/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opning required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    //Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return e_failure;
    }
    else
    //if data is less than root of data, call insert_node recursively with left link
    {
	printf("INFO: Opened %s\n", encInfo->src_image_fname);
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    //Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    	return e_failure;
    }
    else
    {
	printf("INFO: Opened %s\n", encInfo->secret_fname);
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    //Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    	return e_failure;
    }
    else
    {
	printf("INFO: Opened %s\n", encInfo->stego_image_fname);
    }

    // No failure return e_success
    return e_success;
}


/***********************************************************************/
//Close files
void close_files_encode(EncodeInfo *encInfo)
{
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_stego_image);
}

/***********************************************************************/
//Checking secret file size
uint get_file_size(FILE *fptr)
{
    uint file_size;
    fseek(fptr, 0L, SEEK_END);
    file_size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    return file_size;
}

/**********************************************************************/

//Checking image file size
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint image_size;
    fseek(fptr_image, 0L, SEEK_END);
    image_size = ftell(fptr_image);
    fseek(fptr_image, 0L, SEEK_SET);

    return image_size;
}

/***********************************************************************/

//Check capacity
Status check_capacity(EncodeInfo *encInfo)
{
    if(((((long int)encInfo->image_capacity) - 54) - (encInfo->size_secret_file) - (8 * 2) - (8 * 4) - (8 * 4)) < 1)
    {
	return e_failure;
    }
    else
    {
	return e_success;
    }
}

/**********************************************************************/

//Copy header bytes of image into stego_image file
Status copy_bmp_header(FILE *src_image, FILE *dest_image)
{
    char image_header_data[55];
    int read_data, write_data;

    if((read_data = fread(image_header_data, 1, 54, src_image)) != 54)
    {
	return e_failure;
    }
    if((write_data = fwrite(image_header_data, 1, read_data, dest_image)) != read_data)
    {
	return e_failure;
    }
    return e_success;
}


/**********************************************************************/

//Encode magic string
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
//2 bytes * 8 bytes = 16 bytes (55 - 70) for BM Signature

    char magic_msb_bit[16] = {0};
    int i, j, read_data = 0, write_data = 0, iter = 0, temp = 0;

    //Store magic string into buffer
    int magic_string_buf = magic_string[0];
    magic_string_buf = (magic_string_buf << 8) | magic_string[1];

    //Store msb bits
    for(i = 15; i >= 0; i--)
    {
	if((magic_string_buf & 1) == 0)
	{
	    magic_msb_bit[i] = '0';
	}
	else if((magic_string_buf & 1) == 1)
	{
	    magic_msb_bit[i] = '1';
	}
	magic_string_buf = magic_string_buf >> 1;
    }
    magic_msb_bit[8] = '\0';
    printf("Magic_msb_bits = %s\n", magic_msb_bit);

    fseek(encInfo->fptr_src_image, 55L, SEEK_SET);
    fseek(encInfo->fptr_stego_image, 55L, SEEK_SET);

    i = 1;
    while(iter < 71)
    {
	//read byte by byte
	if((read_data = fread(encInfo->image_data, 1, 1, encInfo->fptr_src_image)) != 1)
	{
	    puts("ERROR: Unable to read from file");
	    return e_failure;
	}
	
	//If one byte fetched successfully, check for encoding
	if(encode_byte_tolsb(magic_msb_bit[i], encInfo->image_data) == e_failure)
	{
	    fprintf(stderr, "ERROR: Unable to encode to image");
	    return e_failure;
	}
	
	//encode data stored in buffer is stored to stego image
	if((write_data = fwrite(encInfo->image_data, 1, 1, encInfo->fptr_stego_image)) != 1)
	{
	    fprintf(stderr, "ERROR: Unable to write to stego image");
	    return e_failure;
	}
	
	i++;
	iter++;
    }
    return e_success;
}

/**********************************************************************/

//Encode file extention
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
//4 bytes * 8 bytes = 32 bytes (71 - 102) for .txt


    char extn_msb_bit[32] = {0};
    int i, j, read_data = 0, write_data = 0, iter = 0, temp = 0;
    int extn_string_buf = 0;

    //Store extension into buffer
    for(i = 0; i < 4; i++)
    {
	extn_string_buf |= file_extn[i];
	if(i < 3)
	{
	    extn_string_buf = extn_string_buf << 8;
	}
    }


    //Store msb bits
    for(i = 31; i >= 0; i--)
    {
	if((extn_string_buf & 1) == 0)
	{
	    extn_msb_bit[i] = '0';
	}
	else if((extn_string_buf & 1) == 1)
	{
	    extn_msb_bit[i] = '1';
	}
	extn_string_buf = extn_string_buf >> 1;
    }
    extn_msb_bit[32] = '\0';
    printf("Extn_msb_bits = %s\n", extn_msb_bit);

    fseek(encInfo->fptr_src_image, 71L, SEEK_SET);
    fseek(encInfo->fptr_stego_image, 71L, SEEK_SET);

    i = 1;
    while(iter < 103)
    {
	//read byte by byte
	if((read_data = fread(encInfo->image_data, 1, 1, encInfo->fptr_src_image)) != 1)
	{
	    fprintf(stderr, "ERROR: Unable to read from file");
	    return e_failure;
	}
	
	//If one byte fetched successfully, check for encoding
	if(encode_byte_tolsb(extn_msb_bit[i], encInfo->image_data) == e_failure)
	{
	    fprintf(stderr, "ERROR: Unable to encode to image");
	    return e_failure;
	}
	
	//encode data stored in buffer is stored to stego image
	if((write_data = fwrite(encInfo->image_data, 1, 1, encInfo->fptr_stego_image)) != 1)
	{
	    fprintf(stderr, "ERROR: Unable to write to stego image");
	    return e_failure;
	}
	
	i++;
	iter++;
    }
    return e_success;
}

/*********************************************************************/


//Encode size of file
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // 4 bytes * 8 bytes = 32 bytes (103 - 134)
    char msb_bit[32] = {0};
    int i, j, read_data = 0, write_data = 0, iter = 0, temp = 0;

    //Store msb bits in reverse order
    for(i = 31; i >= 0; i--)
    {
	if((file_size & 1) == 0)
	{
	    msb_bit[i] = '0';
	}
	else if((file_size & 1) == 1)
	{
	    msb_bit[i] = '1';
	}
	file_size = file_size >> 1;
    }
    msb_bit[32] = '\0';
    printf("msb_bit = %s\n", msb_bit);
    

    //seek to 103rd byte src image
    fseek(encInfo->fptr_src_image, 103L, SEEK_SET);

    //seek to 103rd byte of stego image
    fseek(encInfo->fptr_stego_image, 103L, SEEK_SET);

    //Store current position as iteration
    iter = ftell(encInfo->fptr_src_image);

    i = 0;

    while(iter < 135)
    {
	if((read_data = fread(encInfo->image_data, 1, 1, encInfo->fptr_src_image)) != 1)
	{
	    puts("ERROR: Unable to read from file");
	    return e_failure;
	}

//	printf("test1: msb_bit[%d]=%c\n", i, msb_bit[i]);		//for testing
	
	//If one byte fetched successfully, check for encoding
	if(encode_byte_tolsb(msb_bit[i], encInfo->image_data) == e_failure)
	{
	    fprintf(stderr, "ERROR: Unable to encode to image");
	    return e_failure;
	}
	
	//encode data stored in buffer is stored to stego image
	if((write_data = fwrite(encInfo->image_data, 1, 1, encInfo->fptr_stego_image)) != 1)
	{
	    fprintf(stderr, "ERROR: Unable to write to stego image");
	    return e_failure;
	}
	
	i++;
	iter++;
//	printf("test2 msb_bit = %s\n", msb_bit);
    }
    return e_success;
}


/**********************************************************************/

//Encode byte to lsb
Status encode_byte_tolsb(char data, char *image_buffer)
{
    *image_buffer = ((*image_buffer & ~1) | (data & 1));
    return e_success;
}


/*********************************************************************/

//Encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    int read_sec_data = 0, read_data = 0, write_data = 0;
    char temp_data, mask;
    char *temp_msb;
    int sec_data_len = encInfo->size_secret_file;//strlen(encInfo->fptr_secret);
    //printf("secret size = %d\n", sec_data_len);

    //Seek to 0th byte of secret file
    fseek(encInfo->fptr_secret, 0L, SEEK_SET);
    //Seek to 135th byte of image file
    fseek(encInfo->fptr_src_image, 135L, SEEK_SET);
    //Seek to 135th byte of steg file
    fseek(encInfo->fptr_stego_image, 135L, SEEK_SET);

    //Loop untill it reaches EOF of secret file
    while(feof(encInfo->fptr_secret) == '\0')
    {
	//Read 1 byte from secret file store to secret data buffer
	if((read_sec_data = fread(encInfo->secret_data, 1, 1, encInfo->fptr_secret)) != 1)
	{
	    if(ferror(encInfo->fptr_secret) != 0)
	    {
		fprintf(stderr, "ERROR: Unable to read file\n");
		clearerr(encInfo->fptr_secret);
		return e_failure;
	    }
	}
	
	//Get the msb bit of secret file
	temp_data = *encInfo->secret_data;
	//printf("secret data = %c\n", temp_data);
	
	char msb_byte[8] = {0};
	int i, j, read_data = 0, write_data = 0, iter = 0, temp = 0;
	
	//Store msb bits in reverse order
	for(i = 7; i >= 0; i--)
	{
	    if((temp_data & 1) == 0)
	    {
		msb_byte[i] = '0';
	    }
	    else if((temp_data & 1) == 1)
	    {
		msb_byte[i] = '1';
	    }
	    temp_data = temp_data >> 1;
	}
	msb_byte[8] = '\0';

	//printf("msb_byte = %s\n", msb_byte);

	for(i = 0; i < 8; i++)
	{
	    if((read_data = fread(encInfo->image_data, 1, 1, encInfo->fptr_src_image)) != 1)
	    {
		puts("ERROR: Unable to read from file");
		return e_failure;
	    } 
	    
	    if(encode_byte_tolsb(msb_byte[i], encInfo->image_data) == e_failure)
	    {
		fprintf(stderr, "ERROR: Unable to encode to image\n");
		return e_failure;
	    }

	    if((write_data = fwrite(encInfo->image_data, 1, 1, encInfo->fptr_stego_image)) != 1)
	    {
		fprintf(stderr, "ERROR: Unable to write into stego file\n");
		return e_failure;
	    }
	}
    }
    //printf("********** %d\n", ftell(encInfo->fptr_src_image));
	return e_success;
}

/********************************************************************/

//Copy remaining data to stego image file
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int read_data, write_data, i;
    char remaining_data[MAX_IMAGE_BUF_SIZE];
    //printf("size = %ld\n", sizeof(remaining_data));

    fseek(fptr_src, 0L, SEEK_CUR);
    //printf(" cur = %ld\n", ftell(fptr_src));

    while(feof(fptr_src) == 0)
    {
	if((read_data = fread(remaining_data, 1, MAX_IMAGE_BUF_SIZE, fptr_src)) != MAX_IMAGE_BUF_SIZE)
	{
	    if(ferror(fptr_src) != 0)
	    {
	    fprintf(stderr, "ERROR: Unable to copy remaining data\n");
	    return e_failure;
	    }
	}
	
	if((write_data = fwrite(remaining_data, 1, read_data, fptr_dest)) != read_data)
	{
	    return e_failure;
	}
    }
    return e_success;
}
