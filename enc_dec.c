#include<stdio.h>
#include<string.h>
#include"common.h"
#include"types.h"


//Function definitions

//Check operation type
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
	return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }
}

//Check arguments passed
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(check_operation_type(argv) == e_encode)
    {
	//arg for Src Image File
	encInfo->src_image_fname = argv[2];
	if(argv[3] != NULL)
	{
	    //arg for Secret File
	    encInfo->secret_fname = argv[3];
	}
	return e_success;
    }
    else if(check_operation_type(argv) == e_decode)
    {
	//arg for Stego Image
	encInfo->stego_image_fname = argv[2];
	if(argv[3] != NULL)
	{
	    encInfo->secret_fname = argv[3];
	}
	else
	    encInfo->secret_fname = "decode.txt";
	return e_success;
    }
    else if(check_operation_type(argv) == e_unsupported)
    {
	return e_failure;
    }
    return e_failure;
}

