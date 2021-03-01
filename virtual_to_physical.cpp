#include "hw2_src/virtual_to_physical.hpp"

#define BITS_PER_BYTE 8

// Return the virtual page number associated with the given virtual address
//      - page_size : size of pages
//      - addr_bits : number of bits in virtual address
void split_virtual_address(int virtual_address, int page_size, int* VPN, int* offset)
{
    int offset_bits = log2(page_size);
    int vpn_bits = sizeof(int)*BITS_PER_BYTE - offset_bits;

    int mask = 0;
    for (int i = 0; i < vpn_bits; i++)
        mask = mask | (1 << (offset_bits + i));
    *VPN = (virtual_address & mask) >> offset_bits;

    mask = 0;
    for (int i = 0; i < offset_bits; i++)
        mask = mask | (1 << (i));
    *offset = virtual_address & mask;
}





// Split VPN into index and tag
// Assuming k-way associative TLB
void split_VPN(int VPN, int k, int* index, int* tag)
{
    int idx_bits = log2(k);
    int tag_bits = sizeof(VPN)*BITS_PER_BYTE - idx_bits;
    int idx_mask = 0;
    for (int i = 0; i < idx_bits; i++)
        idx_mask = idx_mask | (1 << i);
    int tag_mask = 0;
    for (int i = 0; i < tag_bits; i++)
        tag_mask = tag_mask | (1 << (idx_bits + i));
    *index = (VPN & idx_mask);
    *tag = (VPN & tag_mask) >> idx_bits;
}





// TLB is k-way associative
//     - Find TLB entry, if it exists (return true)
//     - Otherwise, return false
int TLB_lookup(TLB* tlb, int VPN)
{
    int index, tag;
    split_VPN(VPN, tlb->k, &index, &tag);
    
    TLB_entry* entry = NULL;
    bool success = tlb->lookup(index, tag, &entry);
    if (success)
    {
        if (entry->can_access())
        {
            return entry->PFN;
        }
        else
        {
            protection_fault();
        }
    }
    else
    {
        tlb_miss();
    }
    return 0;
}





// Lookup VNP in Page Table
int table_lookup(PageTable* table, TLB* tlb, int VPN)
{
    int index, tag;
    PTE* entry = table->lookup(VPN);
    if (entry->valid_bit)
    {
        if (entry->can_access())
        {
            if (entry->present_bit)
            {
                split_VPN(VPN, tlb->k, &index, &tag);
                tlb->add_entry(index, tag, entry->PFN, entry->protect_bit);
                return entry->PFN;
            }
            else
            {
                page_fault();
            }
        }
        else
        {
            protection_fault();
        }
    }
    else
    {
        segmentation_fault();
    }
    return 0;
}





// Convert PFN and Offset to Physical Address
int get_physical_address(int PFN, int offset, int page_size)
{
    int offset_bits = log2(page_size);

    return ((PFN << offset_bits) | offset);
}





// Put everything together to get physical address from virtual
int virtual_to_physical(int virtual_address, int page_size, TLB* tlb, PageTable* table)
{
    int vpn, offset, pfn;

    split_virtual_address(virtual_address, page_size, &vpn, &offset);

    try
    {
        pfn = TLB_lookup(tlb, vpn);
    }
    catch (const char* msg)
    {
        if (std::string(msg) == "TLB Miss")
            pfn = table_lookup(table, tlb, vpn);
        else throw (msg);
    }

    return get_physical_address(pfn, offset, page_size);
}

