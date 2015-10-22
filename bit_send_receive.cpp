char *bit_send(char *buffer, int cnt){
    int msg_size;
    char *buffer1, *ptr1, c;
    ptr1 = buffer1 = (char*) malloc(8 * cnt);//create a buffer
    while(cnt != 0){       //char is available
        c = *buffer;       //get a char
        cnt--;             //counter decrement
        for(int i = 0; i < 8; i++){ //show each bit as '0' or '1'
            if((c & (0x80>>i)) != 0) {
                *ptr1='1';
            } else {
                *ptr1 = '0';
            }
            ptr1++;
        }
        buffer++;         
    }
    return buffer1;  //return an address of a new string; 
                     //new size is 8 * cnt
}

char *bit_receive(char *buffer, int cnt){
    
    char *buffer1, *ptr, c, c1=0;
    int i = 0;
    if(cnt % 8 != 0) return 0;  //we must have n whole bytes!
    
    ptr = buffer1 = (char*) malloc(cnt/8);

    while(cnt != 0){   
        cnt--;    
        c = *buffer;           

        if(c != '0'){
            c1 |= 0x80 >> i;    
        }
        i++;                  
        if(i>7){                
            i=0;
            *ptr = c1;          
            c1 = 0;             
            ptr++;              
        }
        buffer++;             
    }
    return buffer1; 
}