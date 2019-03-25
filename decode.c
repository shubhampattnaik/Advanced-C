#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "common.h"
#include "types.h"

//Decoding definition

//Open required files
Status open_files_decode(EncodeInfo *encInfo)
{
    //Open Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "r");
    
    if (NULL == encInfo->fptr_stego_image)
    {
        fprintf(stderr, "ERROR: Unable to open %s file\n", encInfo->stego_image_fname);
    	return e_failure;
    }
    return e_success;
}

/************************************************************************/

//Creating decode.txt file
Status create_files_decode(EncodeInfo *encInfo)
{

    //Open Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "w");
    
    if (NULL == encInfo->fptr_secret)
    {
        fprintf(stderr, "ERROR: Unable to open %s file\n", encInfo->secret_fname);
    	return e_failure;
    }
    return e_success;
}

/**********************************************************************/

//Decode Magic String
Status decode_magic_string(FILE *fptr_stego_image)
{
    int read_data = 0, i;
    char magic_string, temp_data;
    char msb_byte[16] = {0};


    fseek(fptr_stego_image, 54L, SEEK_SET);
    
    //Decode next 16 bytes
    for(i = 0; i < 8; i++)
    {
	//Read byte by byte data
	if((read_data = fread(&magic_string, 1, 1, fptr_stego_image)) != 1)
	{
	    //Unable to read
	    if(ferror(fptr_stego_image) != 0)
	    {
		fprintf(stderr, "ERROR: Unable to read from stego image file\n");
		return e_failure;
	    }
	}
	
	//If successfully read 1 byte then decode
	if(decode_byte_from_lsb(&temp_data, &magic_string) == e_failure)
	{
	    fprintf(stderr, "ERROR: Unable to decode magic string\n");
	    return e_failure;
	}
	msb_byte[i] = temp_data;
    }
    msb_byte[i] = '\0';
    
    printf("msb_byte = %s\n", msb_byte);
    
    char c = strtol(msb_byte, 0, 2);
    
    //printf("c = %c\n", c);		//for testing

    if(c == '#')
    {
	printf("INFO: Image is steged\n");
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

/*********************************************************************/

Status decode_secret_file_extn(FILE *fptr_stego_image)
{
    int read_data, write_data, i, j, k;
    
    char msb_data, msb_data2;
    char file_extn_data;
    char *extn;
       
    //set to starting position of secret file
    fseek(fptr_stego_image, 70L, SEEK_SET);

    printf("INFO: FIle extension is: ");
       
    // Decode the data for the file extn
    for(i = 0; i < 4; i++)
    {
        
        //for every 8 bytes we get one byte of data
        for (j = 0; j < 8; j++)
        {
            // To read byte by byte 
            if ((read_data = fread(&file_extn_data, 1, 1, fptr_stego_image)) != 1)
            {
                if (ferror(fptr_stego_image) != 0)
                {
                    fprintf(stderr, "Reading error. \n" );
                    return e_failure;
                }
            }
            
            // Now decode to get LSB
            msb_data = file_extn_data & 01;
            
            // Shift and or
            msb_data2 <<= 1;
            msb_data2 |= msb_data;
        }
	extn[i] = msb_data2;
       
    }
    extn[i] = '\0';
    printf("%s\n", extn);
    return e_success;
}

/*********************************************************************/

// Decode secret file size
uint decode_secret_file_size(FILE * fptr_stego_image)
{
    int read_data, i, j, k, iter, tempnum, temp;
    char msb_byte[32] = {0};
    char tempdata;
    char file_size_data;
    uint file_size = 0;
    
    // Seek to 103rd byte and onward
    fseek(fptr_stego_image, 103L, SEEK_SET);
    
    
    //printf("cur = %ld:", ftell(fptr_stego_image));
    // Decode the next 32 Image data into buffer for size
    for(i = 0; i < 32 ;i++)
    {
	/* To read byte by byte */
        if ((read_data = fread(&file_size_data, 1, 1, fptr_stego_image)) != 1)
        {
            if (ferror(fptr_stego_image) != 0)
            {
                fprintf(stderr, "Reading error. \n" );
                return e_failure;
            }
        }
        
        // Now decode
        if (decode_byte_from_lsb(&tempdata, &file_size_data) == e_failure)
        {
            printf("Decoding of LSb failed.");
            return e_failure;
        }
        msb_byte[i] = tempdata;
    }
    msb_byte[i] = '\0';
    
    //printf("%s\n", msb_byte);				//for testing
    
    file_size = strtol(msb_byte, 0, 2);
    printf("file size = %d\n", file_size);		//for testing
    
    return file_size;
    
}

/* decode a byte into array from LSB of image data */ 

Status decode_byte_from_lsb(char *data, char *image_data)
{
    //get LSB of the data and store in imagebuffer
    //printf("%d", (*image_data & 0x01));		//for testing
    *data = (*image_data & 0x01);
    if ((*image_data & 0x01) == 0)
    {
        *data = '0';
    }
    else if ((*image_data & 0x01) == 1)
    {
        *data = '1';
    }
    
    return e_success;
}

/* Decode secret file data */
Status decode_secret_file_data(FILE * fptr_secret, FILE * fptr_stego_image, long size_secret_file)
{
    //puts("test 1");
    
    int read_data, write_data, i, j;
    
    char msb_data, msb_data2;
    char file_size_data;
    
    //get the current cursor value
    fseek(fptr_stego_image, 0L, SEEK_CUR);
    
    //puts("test 2");
    
    
    //set to starting position of secret file
    fseek(fptr_secret, 0L, SEEK_SET);
    
    //puts("test 3");
    
    // Decode the data for the size_secret_file
    for(i = 0; i < size_secret_file; i++)
    {
        
        //for every 8 bytes we get one byte of data
        for (j = 0; j < 8; j++)
        {
            // To read byte by byte 
            if ((read_data = fread(&file_size_data, 1, 1, fptr_stego_image)) != 1)
            {
                if (ferror(fptr_stego_image) != 0)
                {
                    fprintf(stderr, "Reading error. \n" );
                    return e_failure;
                }
            }
            
            // Now decode to get LSB
            msb_data = file_size_data & 01;
            
            // Shift and or
            msb_data2 <<= 1;
            msb_data2 |= msb_data;
        }
        
	//printf("%c",  msbdata2  );
        
        //now that we have the correct byte, write to secret file
        // If failed to write what is read into dest header
        if ((write_data = fwrite( &msb_data2, 1, 1, fptr_secret)) != 1)
        {
            return e_failure;
        }
        
    }
    
    return e_success;
    
}

/* close files for clean exit */
void close_files_decode(EncodeInfo *encInfo)
{
    fclose(encInfo->fptr_stego_image);
    fclose(encInfo->fptr_secret);
}
