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


#pragma once 
#include <iostream>
#include <vector>
#include <stdint.h>
#include "miniz/miniz.h"
typedef struct rz_pdf_xref rz_pdf_xref;
typedef struct rz_xref_store rz_xref_store;
typedef struct rz_xref_local rz_xref_local;
typedef enum {
    RZ_XREF_TABLE,
    RZ_XREF_STREAM,
    RZ_XREF_MIX, // table and stream pdf was edited
    RZ_XREF_OK,
    rz_XREF_NOT_FOUND,
    RZ_XREF_FAILED,
    RZ_XREF_ERR,
}rz_xref_state;

rz_xref_state table_or_stream(rz_context *ctx);
long lookup_offset (rz_context *ctx, rz_pdf_xref xref, int target_obj);

struct rz_xref_store{
    char status;
    long offset;
    uint32_t obj_id;
    uint32_t obj_stm;
    uint32_t index_obj;
    int generation; 
    uint8_t type;
};

struct rz_xref_local{
    uint32_t obj;
    uint32_t index;
    uint32_t offset;
};

std::vector<rz_xref_store>xref_store(rz_context  *ctx);

struct rz_pdf_xref{
    long start_xref;
    uint32_t size;
    uint16_t count_obj;
    int root_obj;
    std::vector<rz_xref_store>store;
    long total_entries;
    long base_obj;
    rz_xref_state xref_state;
};

rz_xref_state main_xref(rz_context *ctx, rz_pdf_xref &xref);
