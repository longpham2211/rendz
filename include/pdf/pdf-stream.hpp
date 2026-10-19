// STREAM _H

#pragma once 

#include<iostream>
#include <vector>
#include "miniz/miniz.h"
#include "rendz/context.hpp"
typedef struct rz_pdf_stream rz_pdf_stream;

typedef enum {
    RZ_STREAM_OK,
    RZ_STREAM_ERROR,
}rz_stream_state;

std::vector<unsigned char> uncompress_data(rz_context *ctx, 
                                           const unsigned char *source, mz_ulong source_len);

struct rz_pdf_stream{
    rz_stream_state stream_state;
};
