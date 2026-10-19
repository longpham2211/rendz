




#include "rendz/context.hpp"
#include "pdf/pdf-xref.hpp"
#include "pdf/pdf-document.hpp"
#include <iostream>
void find_page(rz_context *ctx, rz_pdf_xref xref){
    std::cout << "HIIIIIII" << "\n";
    long offset = lookup_offset(ctx, xref, xref.root_obj);
    std::cout << "OFFSET ROOT OBJ: " << offset << "\n";
    const unsigned char *root = ctx->doc->file + offset; 
    char buffer[100] = {0};
    for(int i = 0; i < 100; i++){
        buffer[i] = root[i];
    }
    printf("BUFFER: %s\n", buffer);
}

void main_page(rz_context *ctx, rz_pdf_xref xref){
    find_page(ctx, xref);
}
