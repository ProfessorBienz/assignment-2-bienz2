#include "hw2_src/replacement.hpp"

// Delete First Frame in linked list
//
// Return how many frames you stepped through to find replacement frame
int fifo(FrameList* active_frames, FrameList** frame_to_remove_ptr)
{
    *frame_to_remove_ptr = active_frames;
    return 1;
}

// Delete Least Recently Used Frame
// frame->idx shows how recently frame has been used
// higher number == more recently used
//
// Return how many frames you stepped through to find replacement frame
int lru(FrameList* active_frames, FrameList** frame_to_remove_ptr)
{
    int idx;
    int min_idx = active_frames->idx;
    int ctr = 1;
    int min_ctr = ctr;
    FrameList* min_frame = active_frames;
    FrameList* prev_frame = NULL;

    while (active_frames->next != NULL)
    {
        ctr++;
        idx = active_frames->next->idx;
        if (idx < min_idx)
        {
            min_ctr = ctr;
            min_idx = idx;
            min_frame = active_frames->next;
            prev_frame = active_frames;
        }
        active_frames = active_frames->next;
    }

    *frame_to_remove_ptr = min_frame;
    return ctr;
}

// Step through active frames, starting with first element in linked list (passed as arg)
// Remove first active frame with frame->clock_bit set to zero
// Reset any clock_bit as you traverse over a frame
//
// Return how many frames you stepped through to find replacement frame
int clock_lru(FrameList* active_frames, FrameList** frame_to_remove_ptr)
{
    int ctr = 0;
    FrameList* first_frame = active_frames;
    FrameList* prev_frame = NULL;

    while (1)
    {
        ctr++;
        if (active_frames->clock_bit == 0)
        {
            *frame_to_remove_ptr = active_frames;
            break;
        }
        else
        {
            active_frames->clock_bit = 0;
            printf("IDX %d\n", active_frames->idx);
        }

        prev_frame = active_frames;
        active_frames = active_frames->next;
        if (active_frames == NULL)
        {
            prev_frame = NULL;
            active_frames = first_frame;
        }
    }

    return ctr;
}
