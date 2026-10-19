/*=============================================================================
 *
 * Organization: SmallLab28
 * Project: rendz
 * Description: Ultra-fast, zero-copy PDF Extraction & Parsing Engine
 * Version: 0.1.0
 * Origin: Built from scratch
 *
 * Copyright (c) 2026 SmallLab28
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *===========================================================================*/


#include "rendz/context.hpp"
#include "pdf/pdf-document.hpp"
#include <iostream>

#ifdef _WIN32 
#include <windows.h>
rz_doc_state_type open_file (rz_context *ctx, const char *file_name){
    ctx->doc->file_mapping = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, 
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (ctx->doc->file_mapping == INVALID_HANDLE_VALUE){
        printf("ERR MMAP WINDOW:%d", __LINE__);
        return ctx->doc->doc_state = RZ_DOC_OPEN_ERR;
    }
    ctx->doc->mapping = CreateFileMappingA(ctx->doc->file_mapping, NULL, PAGE_READONLY, 0, 0, NULL);
    if(!ctx->doc->mapping) {
        printf("ERR WHILE MAPPING:%d\n", __LINE__);
        return ctx->doc->doc_state = RZ_DOC_MAP_ERR;
    }
    ctx->doc->file = (const unsigned char*)MapViewOfFile(ctx->doc->mapping, FILE_MAP_READ, 0, 0, 0);
    if (ctx->doc->file == NULL) return ctx->doc->doc_state = RZ_DOC_MAP_ERR;
    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(ctx->doc->file_mapping, &file_size)) {
        UnmapViewOfFile(ctx->doc->file);
        CloseHandle(ctx->doc->mapping);
        CloseHandle(ctx->doc->file_mapping);
        ctx->doc->file = NULL;
        return ctx->doc->doc_state = RZ_DOC_SYSTEM_ERR;
    }
    ctx->doc->file_size = static_cast<size_t>(file_size.QuadPart);
    return ctx->doc->doc_state = RZ_DOC_OK; 

}
/*
UnmapViewOfFile(data);
CloseHandle(mapping);
CloseHandle(file);
*/ 
void close_file (rz_context *ctx){
    UnmapViewOfFile(ctx->doc->file);
    CloseHandle(ctx->doc->mapping);
    CloseHandle(ctx->doc->file_mapping);
}
#else
#include <sys/mman.h>
#include <sys/stat.h>
rz_doc_state_type open_file (rz_context *ctx, const char *file_name){
    ctx->doc->fd = open(file_name, O_RDONLY);
    if (ctx->doc->fd == -1) return ctx->doc->doc_state = RZ_DOC_OPEN_ERR;

    struct stat st;
    if (fstat(fd, &st) == -1) {
        printf("ERR FSTAT\n");
        close(ctx->doc->fd);
        return ctx->doc->doc_state = RZ_DOC_SYSTEM_ERR;
    }
    if(!S_ISREG(st.st_mode)) return ctx->doc->doc_state = RZ_DOC_INVALID;
    ctx->doc->file  = (const unsigned char*)mmap
        (NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, ctx->doc->offset);

    ctx->doc->file_size = st.st_size;
    return ctx->doc->doc_state = RZ_DOC_OK;
}

void close_file (rz_context *ctx){
    struct stat st;
    munmap(ctx->doc->file, st.st_size);
    close(ctx->doc->fd);
}
#endif
size_t get_file_size(rz_context  *ctx){
    size_t file_size = ctx->doc->file_size;
    return file_size;
}
rz_doc_state_type check_pdf (rz_context *ctx){
    if (ctx == NULL || ctx->doc == NULL) {
        return RZ_DOC_INVALID;
    }
    if (ctx->doc->file == NULL || ctx->doc->file_size < 4) {
        return ctx->doc->doc_state = RZ_DOC_INVALID;
    }
    if( ctx->doc->file[0] != 0x25 || //%
        ctx->doc->file[1] != 0x50 || //P
        ctx->doc->file[2] != 0x44 || //D
        ctx->doc->file[3] != 0x46){  //F
        return ctx->doc->doc_state = RZ_DOC_INVALID;
    }
    return ctx->doc->doc_state = RZ_DOC_OK;
}

rz_doc_state_type main_doc (rz_context *ctx, const char *file_name){
    rz_doc_state_type status = open_file(ctx, file_name);
    if (status != RZ_DOC_OK) return status;
    return check_pdf(ctx);
}

rz_doc_state_type execute_document (rz_context *ctx, const char *file_name){
    return main_doc(ctx, file_name);
}
