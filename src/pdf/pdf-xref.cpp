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
#include "pdf/pdf-xref.hpp"
#include "miniz/miniz.h"
#include "pdf/pdf-stream.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <cctype>

#define DEFAULT_BUF_SIZE 1024

static void filter_ascii(char *src, char *dest, size_t dest_size) {
    size_t j = 0;
    for (size_t i = 0; i < DEFAULT_BUF_SIZE && j < (dest_size - 1); i++) {
        unsigned char c = static_cast<unsigned char>(src[i]); // for safe cast 
        if (c >= 1 && c <= 126) 
            // 1-126, copying the exact spacing and characters so that the functions below can correctly retrieve the number.
            dest[j++] = static_cast<char>(c); // for safe cast
    }
    dest[j] = '\0';
}

static bool last_word(char *buffer, const char *key_word) {
    char *ptr = buffer;
    char *end = NULL;
    while ((ptr = strstr(ptr, key_word)) != NULL) {
        end = ptr;
        ptr++;
    }
    if (end != NULL) return true;
    else return false;
}

static char *scan_num_char(char *buffer) {
    char *end = buffer;
    while (*end != '\0') {
        if (*end >= '0' && *end <= '9') {
            return end;
            break;
        } else {
            end++;
        }
    }
    return NULL;
}

static char *scan_key_num_char(char *src, const char *key_word) {
    char *cur = strstr(src, key_word);
    while (*cur != '\0') {
        if (*cur >= '0' && *cur <= '9') {
            return cur;
            break;
        } else {
            cur++;
        }
    }
    return NULL;
}

static long scan_num_long(char *buffer) {
    long num = 0;
    char *cur;
    char content[DEFAULT_BUF_SIZE] = {0};
    filter_ascii(buffer, content, DEFAULT_BUF_SIZE);
    char *ptr = content;
    while (*ptr != '\0') {
        if (*ptr >= '0' && *ptr <= '9') {
            num = strtol(ptr, &cur, 10);
            break;
        } else {
            ptr++;
        }
    }
    return num;
}

/*
static void split_chain(rz_context *ctx, char *buffer, const char *start, const char *end, size_t offset_cur, size_t *size_out){
    uint8_t count_key = 1;
    const char *key = end;
    while (*key != '\0') {
        count_key++;
        key++;
    }
    if (count_key < 3) {
        printf("Key word(end parameter) > 3\n");
        return;
    }
    char *start_pos = strstr(buffer, start);
    start_pos += strlen(start);
    char *end_pos = const_cast<char*>(end);
    end_pos -= strlen(end);
    size_t i = (size_t) (start_pos - (char*) ctx->doc->file);
    size_t j = 0;
    size_t count_chain = 0;
    while(ctx->doc->file[i] != *end_pos){
        buffer[j] = (char*) ctx->doc->file[i];
        i++;
        j++;
        count_chain++;
    }
    size_out = count_chain;
}
*/ 

static long scan_key_to_num(char *buffer, const char *key_word) {
    // filter '\0'
    char filter_null[DEFAULT_BUF_SIZE] = {0};
    filter_ascii(buffer, filter_null, DEFAULT_BUF_SIZE);
    char *cur = strstr(filter_null, key_word);
    if (cur == NULL) return 0;

    long num = 0;
    char *end;
    char *last = cur;
    char *next = strstr(cur + 1, key_word);
    while (next != NULL) {
        last = next;
        next = strstr(next + 1, key_word);
    }
    while (*last != '\0') {
        if (*last >= '0' && *last <= '9') {
            num = strtol(last, &end, 10);
            break;
        }
        last++;
    }
    return num;
}

static std::vector<unsigned char> split_chain(rz_context *ctx, const char *buffer, const char *start, const char *end, size_t offset_cur) {
    const char *p = end;
    size_t count_key = 0;
    while (*p != '\0') {
        count_key++;
        p++;
    }
    if (count_key < 3) {
        std::cout << "WARNING (KEY WORD > 3)" << "\n";
        return {};
    }
    long length = scan_key_to_num(const_cast<char*>(buffer), "/Length");
    std::vector<unsigned char> buffer_temp;
    buffer_temp.resize(length);
    char *start_pos = strstr(const_cast<char*>(buffer), start);
    start_pos += strlen(start);
    if (memcmp(end, "endstream", strlen("endstream")) == 0) {
        if (start_pos[0] == '\r' && start_pos[0] == '\n') {
            start_pos += 2;
        } else if (start_pos[0] == '\r' || start_pos[0] == '\n') {
            start_pos += 1;
        }
    }
    size_t offset_end_key = offset_cur + (size_t)(start_pos - buffer);
    size_t j = 0;
    while (j < buffer_temp.size()) {
        if (ctx->doc->file[offset_end_key + 1] == *(end + 1) &&
            ctx->doc->file[offset_end_key + 2] == *(end + 2) &&
            ctx->doc->file[offset_end_key + 3] == *(end + 3))
            break;
        buffer_temp[j] = ctx->doc->file[offset_end_key]; 
        j++;
        offset_end_key++;
    }
    return buffer_temp;
}

static long total_entries(rz_context *ctx, rz_pdf_xref xref) {
    size_t start_read = ctx->doc->file_size - 1024;
    const unsigned char *find_xref = ctx->doc->file + start_read;
    char buffer[1024] = {0};
    memcpy(buffer, find_xref, 1024);
    xref.total_entries = scan_key_to_num(buffer, "/Size");
    return xref.total_entries;
}

// step1: neu xref ton tai theo dang truyen thong thi tim trailer roi bat dau tim prev trong trailerd 
// neu prev co thi nhay den prev do neu nhu xref tiep tuc la dang truyen thong thi lai tim tiep trailer va prev
// neu xref la dang stream thi chi can tim prev den doan endstream va nhay den offset do neu co
// cu nhuu vay cho den khi het xref

static long xref_table(rz_context *ctx, rz_pdf_xref &xref) {
    xref.total_entries = total_entries(ctx, xref);
    xref.store.resize(xref.total_entries);
    const unsigned char *eof = ctx->doc->file + (ctx->doc->file_size - 1024);
    char buffer_temp[1024] = {0};
    memcpy(buffer_temp, eof, 1024);
    filter_ascii(buffer_temp, buffer_temp, DEFAULT_BUF_SIZE);
    long start_xref = scan_key_to_num(buffer_temp, "startxref");
    long prev_xref = scan_key_to_num(buffer_temp, "/Prev");
    printf("prev_xref: %ld\n", prev_xref);
    printf("Xref newest: %ld\n", start_xref);

    if (last_word(buffer_temp, "/Prev")) {
        char *cur;
        const unsigned char* read_xref = ctx->doc->file + start_xref;
        if (memcmp(read_xref, "xref", 4) == 0) {
            xref.root_obj = scan_key_to_num((char*)eof, "/Root");
            printf("Root obj: %d\n", xref.root_obj);
            long obj = strtol(scan_num_char((char*)(ctx->doc->file + start_xref)), &cur, 10);
            printf("obj : %ld\n", obj);
            long entry = strtol(scan_num_char((char*)cur), &cur, 10);
            printf("entry: %ld\n", entry);
            if (obj != 0 && entry != 0) {
                while (*cur != 't') {
                    if (obj != 0) {
                        xref.store[obj].obj_id = obj; 
                        xref.store[obj].offset = strtol(scan_num_char((char*)cur), &cur, 10);
                        xref.store[obj].generation = strtol(scan_num_char((char*)cur), &cur, 10);
                        printf("obj: %d\toffset: %ld\t\n", xref.store[obj].obj_id, xref.store[obj].offset);
                    } else {
                        int count = (int)entry;
                        while (count > 0) {
                            xref.store[obj].obj_id = obj;
                            xref.store[obj].offset = strtol(scan_num_char((char*)cur), &cur, 10);
                            xref.store[obj].generation = strtol(scan_num_char((char*)cur), &cur, 10);
                            count--;
                            obj++;
                        }
                    }
                }
            } 
        }
        while (1) {
            printf("-------HI---------\n"); 
            const unsigned char *offset_temp = ctx->doc->file + prev_xref;
            char buf_temp[1024] = {0};
            filter_ascii((char*)offset_temp, buf_temp, DEFAULT_BUF_SIZE);
            char *ptr = buf_temp;
            if (memcmp(ptr, "xref", 4) == 0) {
                char *end;
                int obj = strtol(scan_num_char((char*)ptr), &end, 10);
                int entry = strtol(scan_num_char((char*)end), &end, 10);
                printf("Base obj: %d\tentry: %d\t\n", obj, entry);
                printf("------START XREF -----\n");
                if (obj != 0) {
                    size_t i = 0;
                    if (entry > 1) {
                        int count = entry;
                        while (count > 0) {
                            xref.store[obj].obj_id = obj;
                            xref.store[obj].offset = strtol(scan_num_char((char*)end), &end, 10);
                            xref.store[obj].generation = strtol(scan_num_char((char*)end), &end, 10);
                            printf("obj: %d\toffset: %ld\t\n", xref.store[obj].obj_id, xref.store[obj].offset);
                            count--;
                            obj++;
                        }
                    }
                } else {
                    int count = entry;
                    while (count > 0) {
                        xref.store[obj].obj_id = obj;
                        xref.store[obj].offset = strtol(scan_num_char((char*)end), &end, 10);
                        xref.store[obj].generation = strtol(scan_num_char((char*)end), &end, 10);
                        printf("obj: %d\toffset: %ld\t\n", xref.store[obj].obj_id, xref.store[obj].offset);
                        obj++;
                        count--;
                    }
                }
            }
            break; //stop loop
        }
    } else {
        size_t eof_offset = ctx->doc->file_size - 1024;
        const unsigned char *eof = ctx->doc->file + eof_offset;
        char buffer[1024] = {0};
        memcpy(buffer, eof, 1024);
        const unsigned char *read_xref = ctx->doc->file + start_xref;
        if (memcmp(read_xref, "xref", 4) == 0) {
            char *cur;
            long obj = strtol(scan_num_char((char*)read_xref), &cur, 10);
            long entry = strtol(scan_num_char((char*)cur), &cur, 10);
            int count = (int) entry;
            printf("OBj: %d\tentry: %d\t\n", obj, entry);
            while (count > 0) {
                xref.store[obj].obj_id = obj;
                xref.store[obj].offset = strtol(scan_num_char((char*)cur), &cur, 10);
                xref.store[obj].generation = strtol(scan_num_char((char*)cur), &cur, 10);
                printf("obj: %d\toffset: %ld\t\n", xref.store[obj].obj_id, xref.store[obj].offset);
                obj++;
                count--;
                //printf("obj: %d\toffset: %ld\tgeneration: %d\t\n", xref.store[obj].obj_id,
                //     xref.store[obj].offset, xref.store[obj].generation);
            }
        } else {
            char *cur = NULL;
            size_t length = scan_key_to_num((char*)read_xref, "/Length");
            size_t index = scan_key_to_num((char*)read_xref, "/Size");
            xref.size = index;
            xref.root_obj = scan_key_to_num((char*)read_xref, "/Root");
            printf("Root obj: %d\n", xref.root_obj);
            size_t f1 = strtol(scan_key_num_char((char*)read_xref, "/W"), &cur, 10);
            size_t f2 = strtol(scan_num_char(cur), &cur, 10);
            size_t f3 = strtol(scan_num_char(cur), &cur, 10);
            printf("f1: %zu\nf2: %zu\nf3: %zu\n", f1, f2, f3);
            size_t row_size = f1 + f2 + f3;
            printf("ROW SIZE: %zu\n", row_size);
            std::vector<unsigned char> raw_data = split_chain(ctx, reinterpret_cast<const char*>(read_xref), "stream", "endstream", start_xref);
            printf("RAW DATA\n0x%02X\n", raw_data.data());
            std::vector<unsigned char> data = uncompress_data(ctx, const_cast<const unsigned char*>(raw_data.data()), raw_data.size()); 
            printf("----- UNCOMRPESSED DATA -------\n");
            for (int i = 0; i < 100; i++) {
                printf("%X\t", data.data()[i]);
            }
            printf("\n"); 
            xref.store.resize(index);
            if (strstr((char*)read_xref, "/Prev") != NULL) {
                uint32_t prev = scan_key_to_num((char*)read_xref, "/Prev");
                const unsigned char *prev_chain = read_xref + prev;
                char buf_prev[DEFAULT_BUF_SIZE] = {0};

            } else {
                for (size_t i = 0, j = 0; j < index; i += row_size, j++) {
                    xref.store[j].obj_id = j;
                    xref.store[j].type = (data[i]);
                    if (xref.store[j].type == 1) {
                        uint32_t byte_offset = f2 * 8;
                        for (size_t k = (f1 + i); k <= (f2 + i); k++) {
                            byte_offset -= 8;
                            xref.store[j].offset |= (long)static_cast<uint32_t>(data[k]) << byte_offset;
                        }
                        uint32_t byte_gen = f3 * 8;
                        uint8_t count_gen = f3;
                        uint32_t count_gen_index = i;
                        while (count_gen > 0) {
                            byte_gen -= 8;
                            xref.store[j].generation |= static_cast<uint32_t>(data[f1 + f2 + count_gen_index]) << byte_gen;
                            count_gen--;
                            count_gen_index++;
                        }
                    } else if (xref.store[j].type == 2) {
                        uint32_t byte_obj = f2 * 8;
                        for (size_t m = (f1 + i); m <= (f2 + i); m++) {
                            byte_obj -= 8;
                            xref.store[j].obj_stm |= static_cast<uint32_t>(data[m]) << byte_obj;
                        }
                        uint32_t byte_index = f3 * 8;
                        uint32_t count_index = f3;
                        uint32_t count_i_index = i;
                        while (count_index > 0) {
                            byte_index -= 8;
                            xref.store[j].index_obj |= static_cast<uint32_t>(data[f1 + f2 + count_i_index]) << byte_index;
                            count_index--;
                            count_i_index++;
                        }
                    }
                    xref.count_obj++;
                }
            }
        }
    }
    int count = 0;
    for (int i = 0; i < xref.count_obj; i++) {
        printf("Type: %u\tObj: %d\toffset %zu \t|obj_stm: %u\tgeneration: %d \t| index_obj: %u\n", 
               xref.store[i].type, xref.store[i].obj_id, 
               xref.store[i].offset, xref.store[i].obj_stm, 
               xref.store[i].generation, xref.store[i].index_obj);
        count++;
    }
    printf("COUNT XREF: %d\n", count);
    printf("SUCCESS\n");
    return 1; //prototype
}

rz_xref_state table_or_stream(rz_context *ctx) {
    auto find_all_xref = [=]() {
        const unsigned char *find_xref = ctx->doc->file + (ctx->doc->file_size - 1024);
        char buffer[1024] = {0};
        memcpy(buffer, find_xref, 1024);
        filter_ascii(buffer, buffer, DEFAULT_BUF_SIZE);
        long start_xref = scan_key_to_num(buffer, "startxref");
        long prev_chain_offset = 0;
        char buffer_chain[1024] = {0};
        memcpy(buffer_chain, buffer, 1024);
        unsigned char *prev_chain = NULL;
        uint8_t count_prev = 0;
        uint8_t count_xref = 1;
        while (strstr(buffer_chain, "/Prev") != NULL) {
            prev_chain_offset = scan_key_to_num(buffer_chain, "/Prev");
            prev_chain = (unsigned char*)(ctx->doc->file + prev_chain_offset);
        
            for (size_t i = 0; i < DEFAULT_BUF_SIZE; i++) {
                if (prev_chain[i] == '%' && prev_chain[i + 1] == '%' && prev_chain[i + 2] == 'E') {
                    buffer_chain[i] = '\0';
                    break;
                }
                buffer_chain[i] = prev_chain[i];
            }
            count_prev++;
            count_xref++;
        }
        printf("So luong prev la: %u\n", count_prev);
        printf("So luong xref hien tai la: %u\n", count_xref);
        return start_xref;
    };
    auto check = [=]() {
        const unsigned char *check_xref = ctx->doc->file + find_all_xref();
        if (memcmp(check_xref, "xref", 4)) return RZ_XREF_STREAM;
        else return RZ_XREF_TABLE;
    };
    if (check() == RZ_XREF_STREAM) printf("PDF dang xref stream\n");
    else printf("PDF dang traditional\n");
    return RZ_XREF_OK;
}

long lookup_offset(rz_context *ctx, rz_pdf_xref xref, int target_obj){
    return xref.store[target_obj].offset;
}
std::vector<rz_xref_store> xref_store(rz_context *ctx, rz_pdf_xref &xref) {
    xref.store.resize(xref.total_entries);
    size_t start_read = ctx->doc->file_size - 1024;
    const unsigned char *find_xref = ctx->doc->file + start_read;
    char buffer[1024] = {0};
    for (size_t i = 0; i < 1024; i++) {
        buffer[i] = find_xref[i];
    }

    printf("%ld", xref_table(ctx, xref));
    return xref.store;
}

void execute_xref(rz_context *ctx, rz_pdf_xref xref) {
    xref_store(ctx, xref);
}

rz_xref_state main_xref(rz_context *ctx, rz_pdf_xref &xref) {
    execute_xref(ctx, xref);
    return table_or_stream(ctx);
}
