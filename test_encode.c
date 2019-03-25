#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"

int main(int argc, char **argv)
{
    EncodeInfo encInfo;
    uint img_size;
    uint sec_size;
    char magic_string;
    //long size_secret_file;

    int arg_count = argc;

    char *option = argv[1];

    // Fill with sample filenames
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";
   

    //Check and validate arguments
    if(read_and_validate_encode_args(argv, &encInfo) == e_failure)
    {
	fprintf(stderr, "ERROR: Invalid Arguments\n");
	return 1;
    }

    //Check operation type
    if(check_operation_type(argv) == e_encode)
    {
	// Test open_files
	if (open_files(&encInfo) == e_failure)
	{
	    printf("ERROR: %s function failed\n", "open_files" );
	    return 1;
	}
	else
	{
	    printf("SUCCESS: %s function completed\n", "open_files" );
	}
	
	/*    // Test get_image_size_for_bmp
	      img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
	      printf("INFO: Image size = %u\n", img_size);*/
	
	puts("INFO: ## Encoding procedure started ##");
	
	/*if(get_sec_file_size(&encInfo) == e_failure)
	  {
	  printf("ERROR: %s function failed\n", "get_sec_file_size");
	  return 1;
	  }*/
	
	encInfo.size_secret_file = get_file_size(encInfo.fptr_secret);
	printf("INFO: Secret file size = %lu\n", encInfo.size_secret_file);
	
	encInfo.size_image_file = get_image_size_for_bmp(encInfo.fptr_src_image);
	printf("INFO: Image file size = %lu\n", encInfo.size_image_file);
	
	
	//Check for capacity
	printf("INFO: Checking for %s capacity to handle %s\n", encInfo.src_image_fname, encInfo.secret_fname);
	if(check_capacity(&encInfo) == e_failure)
	{
	    printf("Capacity of %s is less\n", encInfo.src_image_fname);
	    close_files_encode(&encInfo);
	    return 1;
	}
	else
	{
	    printf("INFO: Done. Found OK\n");
	}
	
	//Copy header bytes
	puts("INFO: Copying Image Header");
	if(copy_bmp_header(encInfo.fptr_src_image, encInfo.fptr_stego_image) == e_failure)
	{
	    printf("ERROR: Unable to copy header to %s\n", encInfo.stego_image_fname);
	    return 1;
	}
	else
	{
	    puts("INFO: Done");
	}
	
	//Encode magic string
	printf("INFO: Encoding Magic String Signature: %s\n", MAGIC_STRING);
	if(encode_magic_string(MAGIC_STRING, &encInfo) == e_failure)
	{
	    printf("ERROR: Unable to copy magic string to %s\n", encInfo.stego_image_fname);
	    return 1;
	}
	else
	{
	    puts("INFO: Done");
	}
	
	//Encode file extention
	printf("INFO: Encoding %s File Extension\n", encInfo.secret_fname);
	if(encode_secret_file_extn(EXTENSION, &encInfo) == e_failure)
	{
	    fprintf(stderr, "ERROR: Unable to copy %s file extension to %s\n", encInfo.secret_fname, encInfo.stego_image_fname);
	    return 1;
	}
	else
	{
	    puts("INFO: Done");
	}
	
	//Encode secret file size
	printf("INFO: Encoding %s file size\n", encInfo.secret_fname);
	if(encode_secret_file_size(encInfo.size_secret_file, &encInfo) == e_failure)
	{
	    printf("ERROR: Unable to encode secret file size \n");
	    close_files_encode(&encInfo);
	    return 1;
	}
	else
	{
	    puts("INFO: Done");
	}
	
	//Encode secret file data
	printf("INFO: Encoding %s file data\n", encInfo.secret_fname);
	if(encode_secret_file_data(&encInfo) == e_failure)
	{
	    printf("ERROR: Unable to encode file data\n");
	    close_files_encode(&encInfo);
	    return 1;
	}
	else
	{
	    puts("INFO: Done");
	}
	
	//Copy remaining data
	printf("INFO: Copying remaining data into %s\n", encInfo.stego_image_fname);
	if(copy_remaining_img_data(encInfo.fptr_src_image, encInfo.fptr_stego_image) == e_failure)
	{
	    fprintf(stderr, "ERROR: Unable to copy remaining data\n");
	    close_files_encode(&encInfo);
	    return 1;
	}
	else
	{
	    close_files_encode(&encInfo);
	    puts("INFO: Done");
	    puts("INFO: ## Encoding Done Successfully ##");
	}
    }
  
/**************************************************************************************************/
    else if(check_operation_type(argv) == e_decode)
    {
	printf("INFO: ## Decoding Procedure Started ##\n");

	//Open files
	printf("INFO: Opening required files\n");
	if(open_files_decode(&encInfo) == e_failure)
	{
	    fprintf(stderr, "ERROR: Unable to open files\n");
	    return 1;
	}
	else
	{
	    printf("INFO: Opened %s\n", encInfo.stego_image_fname);
	}

	//Check magic string
	printf("INFO: Decoding Magic String Signature\n");
	if(decode_magic_string(encInfo.fptr_stego_image) == e_failure)
	{
	    fprintf(stderr, "ERROR: Image is not steged\n");
	    return 1;
	}
	else
	{
	    puts("INFO: Done");
	}
	
	//Decode output file extension
	printf("INFO: Decoding File Extension\n");
	if(decode_secret_file_extn(encInfo.fptr_stego_image) == e_failure)
	{
	    fprintf(stderr, "ERROR: Unable to fetch file extension\n");
	    return 1;
	}
	else
	{
	    puts("INFO: Done");
	}

	//check for output file in args
	if(argv[3] == NULL)
	{
	    printf("INFO: Output File not mentioned. Creating decode.txt as default");
	    encInfo.secret_fname = "decode.txt";
	    
	    if(create_files_decode(&encInfo) == e_failure)
	    {
		fprintf(stderr, "ERROR: Unable create decode.txt\n");
		return 1;
	    }
	    else
	    {
		printf("INFO: Opened %s\n", encInfo.secret_fname);
		printf("INFO: Done. Opened all required files\n");
	    }
	}
	else
	{
	    encInfo.secret_fname = argv[3];
	    if(create_files_decode(&encInfo) == e_failure)
	    {
		fprintf(stderr, "ERROR: Unable to open file\n");
		return 1;
	    }
	    else
	    {
		printf("INFO: Opened %s\n", encInfo.secret_fname);
	    }
	}

	//check file size
	printf("INFO: Decoding File Size\n");
	encInfo.size_secret_file = decode_secret_file_size(encInfo.fptr_stego_image);
	//printf("size sec = %ld\n", encInfo.size_secret_file);
	puts("INFO: Done");
	

	//decode secret data
	printf("INFO: Decoding File Data\n");
	
	//printf("size sec = %ld\n", encInfo.size_secret_file);
	if(decode_secret_file_data(encInfo.fptr_secret, encInfo.fptr_stego_image, encInfo.size_secret_file) == e_failure)
	{
	    fprintf(stderr, "ERROR: Unable to decode secret data\n");
	    close_files_decode(&encInfo);
	    return 1;
	}
	else
	{
	    close_files_decode(&encInfo);
	    puts("INFO: Done");
	    puts("INFO: ## Decoding Done Successfully ##");
	}
    }
    return 0;
}
