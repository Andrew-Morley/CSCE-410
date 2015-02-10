#include "file_system.H"
#include "utils.H"
#include "console.H"
#define MAXFILE 40

unsigned int File::random = 0;
FileSystem* File::file_system = 0;

File::File() {
    	position = block = 0;
        file_id = random; random ++;
        BOOLEAN found = file_system->LookupFile(file_id, this);

        /* If not found already and able to create */
        if(!found && file_system->CreateFile(file_id)) {
                size = 0;
                start_block = 0;
        }
}

unsigned int File::Read(unsigned int _n, char * _buf) {
    	if(_n==0 || EoF() || size == 0)	return 0;

        /*
          I will assume that I can read at most 512 bytes */
        
        char temp_buf[512];
        unsigned int char_read = 0; /* How many chars I have read */
		
        while(_n) {

                file_system->disk->read(block, temp_buf);
			
                unsigned int offset = position % 508;
                /* minimum of _n and available bytes in block and size - position */
                unsigned int data_read;
                if (_n > 508 - offset)
                        data_read = 508 - offset;
                else
                        data_read = _n ;
                
                /* Cant to more than I have */
                if(data_read > size - position)
                        data_read = size - position;
                
                memcpy(_buf + char_read, temp_buf + offset, data_read);

                /* Update the variables */
                char_read += data_read;
                _n -= data_read;
                position += data_read;

                /* if I am done with the block, move to the next one */
                if((position % 508 == 0) && !EoF()) {
                        file_system->disk->read(block, temp_buf);
                        memcpy(&block, temp_buf+508, 4);
                }
        }
		
        return char_read;
}

unsigned int File::Write(unsigned int _n, char * _buf) {
    	if(_n == 0) return 0;

        char temp_buf[512];
        unsigned int char_written = 0;
		
        while(_n) {

                /* Similar proceedure as read () */
                unsigned int offset = position % 508;
                file_system->disk->read(block, temp_buf);
                unsigned int data_write;
                if (_n > 508 - offset)
                        data_write = 508 - offset;
                else
                        data_write = _n ;
                memcpy(temp_buf + offset, _buf + char_written, data_write);
                file_system->disk->write(block, temp_buf);

                /* Make sure that size is updated */
                if(data_write + position > size)
                        size = position + data_write;

                /* Now update the common variables */
                char_written += data_write;
                _n -= data_write;
                position += data_write;

                /* Do I need to switch to next block ? */
                if((position % 508 == 0) && !EoF()) {
                        file_system->disk->read(block, temp_buf);
                        memcpy(&block, temp_buf+508, 4);
                }
        }
        /*
          flush it */
        return char_written;
}

void File::Reset() {
    	position = 0;
        block = start_block;
}

void File::Rewrite() {
    	position = 0;
        block = 0;

        char temp_buf[512];
        unsigned int temp_block = start_block;
        unsigned int next_block = 0;

        /* Do for all blocks */
        while(1) {
                /* If done */
                if((int)block == -1) {
                        break;
                }

                /* Read and copy */
                file_system->disk->read(temp_block, temp_buf);
                memcpy(&next_block, temp_buf+508, 4);

                /* Copy and write */
                memcpy(temp_buf+508, &(file_system->free_block), 4);
                file_system->disk->write(temp_block, temp_buf);

                file_system->free_block = temp_block;

                /* Go to the next block */
                temp_block = next_block;
        }
        
        /* Reset the variables */
        start_block = 0;
        size = 0;
}

BOOLEAN File::EoF() {
    	return size == position;
}

FileSystem::FileSystem() {
        disk = NULL;
        size = 0;
        free_block = 0;
        number_of_files = 0;
        File::file_system = this;
}

BOOLEAN FileSystem::Mount(SimpleDisk * _disk) {
        disk = _disk;
        char _buf[512] = {0,};
        disk->read(0, _buf);
        memcpy(&size, _buf, 4);
        memcpy(&free_block, _buf+4, 4);
        memcpy(&number_of_files, _buf+8, 4);
        memcpy(file_data, _buf + FS_DATA_OFFSET, 3 * 4 * MAXFILE);
        return TRUE;
}

BOOLEAN FileSystem::Format(SimpleDisk * _disk, unsigned int _size) {
        
        char _buf[512] = {0,};
		
        //initialize the FS reserved blocks except the first block
        for(unsigned int block_no = 1; block_no < FS_BLOCK_LENGTH; block_no++) {
                _disk->write(block_no, _buf);
        }

        //initialize the free blocks (except the last block) assuming _size is a multiple of BLOCK_SIZE
        unsigned int block_no = FS_BLOCK_LENGTH;
        for(; block_no < _size / BLOCK_SIZE - 1; block_no++) {
                unsigned int next_block = block_no +1;
                memcpy(_buf + 508, &next_block, 4);
                _disk->write(block_no, _buf);
        }

        //initialize the last block
        int last_block_limiter = -1;
        memcpy(_buf + 508, &last_block_limiter, 4);
        _disk->write(block_no, _buf);
		

        //initialize the first block
        unsigned int temp = _size;
        memcpy(_buf+0, &temp, 4); 				//disk size
        temp = FS_BLOCK_LENGTH; 				//first free block
        memcpy(_buf+4, &temp, 4);
        temp = 0; 								//number of files
        memcpy(_buf+8, &temp, 4);
        memset(_buf+12, 0, 500);				//File data -- necessary to format as file_id == 0 is considered as a hole for filling in file data
        _disk->write(0, _buf);

}

BOOLEAN FileSystem::LookupFile(int _file_id, File * _file) {
   
        if(_file_id == 0 || _file == NULL)
                return FALSE;

        /* Check if it exists */
        for(unsigned int file_counter = 0; file_counter < MAXFILE; file_counter++) {
                if(file_data[file_counter][0] == _file_id) {
                        _file->size = file_data[file_counter][1];
                        _file->start_block = file_data[file_counter][2];
                        return TRUE;
                }
        }
}

BOOLEAN FileSystem::CreateFile(int _file_id) {
        char temp_buf[512];
        unsigned int file_counter;
        /* Check if the file already exists */
        for(file_counter = 0; file_counter < MAXFILE; file_counter++) { //check all MAXFILE file data due to holes
                if(file_data[file_counter][0] == _file_id) //file exists already
                        return FALSE;
        }

        /* the file does not exists */
        /* Check if there is an empty entry */
        for(file_counter = 0; file_counter < MAXFILE; file_counter++) {
                if(file_data[file_counter][0] == 0)
                        break;
        }

        /* Add it into the array */
        file_data[file_counter][0] = _file_id;
        file_data[file_counter][1] = 0; //initial size
        file_data[file_counter][2] = 0; //initial starting block
        number_of_files++;
        return TRUE;		
}

BOOLEAN FileSystem::DeleteFile(int _file_id) {
        unsigned int file_counter;

        /* Find the file and see if it exists */
        for(file_counter = 0; file_counter < MAXFILE; file_counter++) {
                if(file_data[file_counter][0] == _file_id)
                        break;
        }
        if(file_counter == MAXFILE) return FALSE;

        /* The file exists */
        
        char temp_buf[512];

        /* get the starting block */
        unsigned int block = file_data[file_counter][2];
        unsigned int next_block = 0;
        while(1) {

                /* If done then break */
                if((int)block == -1) {
                        break;
                }

                /* Read and copy */
                disk->read(block, temp_buf);
                memcpy(&next_block, temp_buf+508, 4);

                /* Copy and write */
                memcpy(temp_buf+508, &free_block, 4);
                disk->write(block, temp_buf);
                
                free_block = block;
                block = next_block;
        }

        /* empty the entries */
        file_data[file_counter][0] = 0;
        file_data[file_counter][1] = 0;
        file_data[file_counter][2] = 0;

        return TRUE;
}
