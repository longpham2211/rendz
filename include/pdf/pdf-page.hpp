
#pragma once
#include "rendz/context.hpp"
#include "pdf/pdf-xref.hpp"

void find_page(rz_context *ctx, rz_pdf_xref xref);
void main_page(rz_context *ctx,rz_pdf_xref xref);
