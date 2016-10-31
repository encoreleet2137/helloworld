#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#pragma warning (disable : 4996) //fread,strcpy insecure

#ifdef _DEBUG
#pragma warning (disable : 4710) //function not inlined
#endif


#define CHARSET_LENGTH ((1<<(sizeof(char)*8))-1)


int check_input ( int argc , char** argv ) {
	if ( argc < 3 ) {
		printf ( "Usage: %s <filename> <compression/decompression>\ncompression - c, decompression - d\n%s file.txt d\n" , argv [ 0 ] , argv [ 0 ] );
		return -1;
	}

	return 0;
}

size_t filesize ( FILE* f ) {
	fseek ( f , 0 , SEEK_END );
	size_t size = ftell ( f );
	fseek ( f , 0 , SEEK_SET );
	return size;
}

size_t *byteHistogram ( char* input , size_t length ) {

	size_t *histogram = calloc ( CHARSET_LENGTH + 1 , sizeof ( size_t ) ) , i;
	for ( i = 0; i < length; i++ )
		++histogram [ ( int )input [ i ] ];
	return histogram;
}

size_t compress ( char* inputbuffer , size_t length , char** outputbuffer ) {
	*outputbuffer = 0;
	size_t* histogram = byteHistogram ( inputbuffer , length ) , i;
	for ( i = 0; i < CHARSET_LENGTH + 1; i++ )
		printf ( "%zu (%c) | %zu\n" , i , ( unsigned char )i , histogram [ i ] );
	return 0;
}

size_t decompress ( char* inputbuffer , size_t length , char** outputbuffer ) {
	*outputbuffer = 0;
	inputbuffer = 0;
	if ( length > 3 ) { }
	return 0;
}

enum huffMode {
	UNKNOWN = -1 ,
	COMPRESS ,
	DECOMPRESS
};


int processMode ( const char* param ) {
	
	size_t len = strlen ( param );
	if ( len == 1 ) {
		switch ( param [ 0 ] ) {
			case 'c': return COMPRESS;
			case 'd': return DECOMPRESS;
			default: return UNKNOWN;
		}
	}
	return UNKNOWN;
}

int main ( int argc , char** argv ) {

	int result = check_input ( argc , argv );
	if ( result != 0 ) return 0;

	//mode 1 - compress, 0 - decompress
	int mode = processMode ( argv [ 2 ] );


	FILE* f = fopen ( argv [ 1 ] , "r" );

	if ( f == NULL ) {
		printf ( "Cannot open file %s!\n" , argv [ 2 ] );
		return 0;
	}


	size_t fileSize = filesize ( f );

	printf ( "File %s succesfully opened, size: %zu\n" , argv [ 2 ] , fileSize );

	if ( fileSize == 0 ) {
		fclose ( f );
		printf ( "Error, file is empty!\n" );
		return 0;
	}

	char* buffer = malloc ( fileSize * sizeof ( char ) );

	if ( fread ( buffer , 1 , fileSize , f ) == 0 ) {
		fclose ( f );
		printf ( "Error while reading file %s!\n" , argv [ 2 ] );
		return 0;
	}

	puts ( "File has been succesfully read into buffer" );

	fclose ( f );

	char* compressed_buffer = NULL;
	size_t result_size = 0;

	switch ( mode ) {
		case COMPRESS:	result_size = compress ( buffer , fileSize , &compressed_buffer );
		case DECOMPRESS: result_size = decompress ( buffer , fileSize , &compressed_buffer );
		default: result_size = 0;
	}

	if ( result_size == 0 || compressed_buffer == NULL ) {
		printf ( "Error while %s input file!" , mode == 1 ? "compressing" : "decompressing" );
		return 0;
	}
	char* modestring = mode == 1 ? "_compressed" : "_decompressed";
	char* outname = malloc ( strlen ( argv [ 2 ] ) + strlen ( modestring ) );
	strcpy ( outname , argv [ 1 ] );
	strcat ( outname , modestring );

	FILE* outfile = fopen ( outname , "wt" );

	if ( outfile == NULL ) {
		printf ( "Cannot open output file %s!\n" , outname );
		return 0;
	}

	if ( fwrite ( compressed_buffer , 1 , result_size , outfile ) == 0 ) {
		printf ( "Error while writing to output file!\n" );
		return 0;
	}

	printf ( "Succesfully %scompressed file %s, output file: %s, compression ratio: %.2f%% (%zu/%zu)\n" ,
			 mode == 0 ? "de" : "" , argv [ 1 ] , outname , ( ( float )result_size / fileSize ) * 100 , result_size , fileSize );

	fclose ( outfile );
	free ( compressed_buffer );
	free ( outname );
	free ( buffer );


	return 0;
}
