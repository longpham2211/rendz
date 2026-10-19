// STREAM_CPP 


#include <stdio.h>
#include <vector>
#include "miniz/miniz.h"
#include "pdf/pdf-stream.hpp"
#include "rendz/context.hpp"


std::vector<unsigned char> uncompress_data(rz_context *ctx, 
                                           const unsigned char *source, mz_ulong source_len){
        std::vector<unsigned char> dest;
        dest.resize(static_cast<size_t>(source_len) * 2);
        while(true){
            mz_ulong dest_len = (mz_ulong) dest.size();
            int status = mz_uncompress(dest.data(), &dest_len, source, source_len);
            if (status == MZ_OK){
                dest.resize(dest_len);
                break;
            }
            else if (status == MZ_BUF_ERROR){
                dest.resize(dest.size() * 2);
            }
            else {
                printf("MINIZ CODE: %d\n", status);
                dest.clear();
                break;
            }
        }
    return dest;
}




