#ifndef __CACHE_H__
#define __CACHE_H__

struct cache_line
{
    uint8_t valid_bit;
    unsigned tag;
    uint8_t cache_block[64];
};

typedef struct L1_cache
{
    struct cache_line L1_cache_line[1024];
    unsigned (* op1)(struct L1_cache *this,unsigned arg);
    void (* op2)(struct L1_cache *this,unsigned arg,unsigned n,unsigned length);
    unsigned (* op3)(struct L1_cache *this,unsigned address);
}L1_cache;

L1_cache obj;

unsigned get_byte(L1_cache *this,unsigned address)
{
    unsigned set_index=(address/64)%128;
    unsigned tag_target=address/(1024*8);
    unsigned block_offset=address%64;
    int i=0;
    for(;i<8;i++)
    {
        if(this->L1_cache_line[set_index*8+i].tag==tag_target)
        {
            return this->L1_cache_line[set_index*8+i].cache_block[block_offset];
        }
    }
    return -1;
}

bool update_write_through(L1_cache *this,unsigned address,unsigned data,unsigned length)
{
    unsigned set_index=(address/64)%128;
    unsigned tag_target=address/(1024*8);
    unsigned block_offset=address%64;
    if(block_offset+length-1>=64)assert(0);/*if exceed the block*/
    int i=0;
    for(;i<8;i++)
    {
        if(this->L1_cache_line[set_index*8+i].tag==tag_target)
        {
            int j=0;
            while(j<length)
            {
                this->L1_cache_line[set_index*8+i].cache_block[block_offset+j]=data%256;
                data>>=8;
                j++;
            }
            return true;
        }
    }
    return false;
}

unsigned L1_replace 


obj.op1=get_byte;
obj.op2=update_write_through;




