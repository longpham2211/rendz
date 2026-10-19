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


#include "pdf/pdf-document.hpp"
#include "pdf/pdf-xref.hpp"
#include "rendz/context.hpp"
#include "pdf/pdf-page.hpp"
#include <iostream>
using namespace std;
int main (){
    rz_context *ctx = init_rz_context();
    if (ctx == NULL) return 0;
#ifdef __WIN32
    const char *file_name = "C:\\rendz\\data\\perfect.pdf";
#else 
    const char *file_name = "../data/a.pdf";
#endif 
    rz_doc_state_type status = execute_document(ctx, file_name);
    printf("File size is: %zu bytes\n", get_file_size(ctx));
    rz_pdf_xref xref{};
    main_xref(ctx, xref);
    main_page(ctx, xref);
    if (ctx->doc->file != NULL) {
        close_file(ctx);
    }

    drop_rz_context(ctx);
    return status;
}   
