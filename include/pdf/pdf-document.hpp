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
#ifndef RZ_DOCUMENT_H
#define RZ_DOCUMENT_H


#ifdef _WIN32 
#include <windows.h>
#endif

#include <iostream>
#include "rendz/context.hpp"

typedef struct rz_document rz_document;

typedef enum {
    RZ_DOC_OK = 0,        // Document is valid and ready.
    RZ_DOC_OPEN_ERR,      // The input file could not be opened.
    RZ_DOC_MAP_ERR,       // The file could not be mapped into memory.
    RZ_DOC_SYSTEM_ERR,    // A platform or filesystem operation failed.
    RZ_DOC_INVALID,       // The input is not a valid PDF document.
    RZ_DOC_UNSUPPORTED,   // The PDF feature or version is not supported.
    RZ_DOC_PARSE_ERR,     // PDF syntax or object parsing failed.
    RZ_DOC_XREF_ERR,      // Cross-reference data is missing or invalid.
    RZ_DOC_STREAM_ERR,    // A PDF stream could not be decoded or read.
    RZ_DOC_INTERNAL_ERR,  // An unexpected library invariant failed.
} rz_doc_state_type;

rz_doc_state_type open_file (rz_context *ctx, const char *file_name);
void close_file (rz_context *ctx);
rz_doc_state_type check_pdf (rz_context *ctx);
rz_doc_state_type main_doc (rz_context *ctx, const char *file_name);
rz_doc_state_type execute_document (rz_context *ctx, const char *file_name);
size_t get_file_size(rz_context *ctx);

struct rz_document {
#ifdef _WIN32 
    HANDLE mapping;
    HANDLE file_mapping;
#endif
    int fd;
    size_t file_size;
    const unsigned char *file;
    size_t offset;

    rz_doc_state_type doc_state;
};

#endif



