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
#include <new>
void drop_rz_context (rz_context *ctx){
    if (ctx == NULL) return;
    delete ctx->doc;
    delete ctx;
}

rz_context *init_rz_context (void){
    rz_context *ctx = new (std::nothrow) rz_context {};
    if(ctx == NULL){ 
        delete ctx;
        return NULL;
    }
    //doc
    ctx->doc = new(std::nothrow) rz_document{};
    if(ctx->doc == NULL){
        delete ctx->doc;
        delete ctx;
        return NULL;
    }
    //doc 
    ctx->doc->file = nullptr;
    ctx->doc->offset = 0;
    ctx->doc->file_size = 0;
    return ctx;
}
