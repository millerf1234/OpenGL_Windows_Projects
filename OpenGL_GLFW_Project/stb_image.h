
tga_data[index2] = temp;
++index1;
++index2;
            }
         }
      }
      //   clear my palette, if I had one
      if (tga_palette != NULL)
      {
          STBI_FREE(tga_palette);
      }
   }

   // swap RGB - if the source data was RGB16, it already is in the right order
   if (tga_comp >= 3 && !tga_rgb16)
   {
       unsigned char* tga_pixel = tga_data;
       for (i = 0; i < tga_width * tga_height; ++i)
       {
           unsigned char temp = tga_pixel[0];
           tga_pixel[0] = tga_pixel[2];
           tga_pixel[2] = temp;
           tga_pixel += tga_comp;
       }
   }

   // convert to target component count
   if (req_comp && req_comp != tga_comp)
       tga_data = stbi__convert_format(tga_data, tga_comp, req_comp, tga_width, tga_height);

   //   the things I do to get rid of an error message, and yet keep
   //   Microsoft's C compilers happy... [8^(
   tga_palette_start = tga_palette_len = tga_palette_bits =
       tga_x_origin = tga_y_origin = 0;
   //   OK, done
   return tga_data;
}
#endif

// *************************************************************************************************
// Photoshop PSD loader -- PD by Thatcher Ulrich, integration by Nicolas Schulz, tweaked by STB

#ifndef STBI_NO_PSD
static int stbi__psd_test(stbi__context* s)
{
    int r = (stbi__get32be(s) == 0x38425053);
    stbi__rewind(s);
    return r;
}

static int stbi__psd_decode_rle(stbi__context* s, stbi_uc* p, int pixelCount)
{
    int count, nleft, len;

    count = 0;
    while ((nleft = pixelCount - count) > 0) {
        len = stbi__get8(s);
        if (len == 128) {
            // No-op.
        }
        else if (len < 128) {
            // Copy next len+1 bytes literally.
            len++;
            if (len > nleft) return 0; // corrupt data
            count += len;
            while (len) {
                *p = stbi__get8(s);
                p += 4;
                len--;
            }
        }
        else if (len > 128) {
            stbi_uc   val;
            // Next -len+1 bytes in the dest are replicated from next source byte.
            // (Interpret len as a negative 8-bit int.)
            len = 257 - len;
            if (len > nleft) return 0; // corrupt data
            val = stbi__get8(s);
            count += len;
            while (len) {
                *p = val;
                p += 4;
                len--;
            }
        }
    }

    return 1;
}

static void* stbi__psd_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri, int bpc)
{
    int pixelCount;
    int channelCount, compression;
    int channel, i;
    int bitdepth;
    int w, h;
    stbi_uc* out;
    STBI_NOTUSED(ri);

    // Check identifier
    if (stbi__get32be(s) != 0x38425053)   // "8BPS"
        return stbi__errpuc("not PSD", "Corrupt PSD image");

    // Check file type version.
    if (stbi__get16be(s) != 1)
        return stbi__errpuc("wrong version", "Unsupported version of PSD image");

    // Skip 6 reserved bytes.
    stbi__skip(s, 6);

    // Read the number of channels (R, G, B, A, etc).
    channelCount = stbi__get16be(s);
    if (channelCount < 0 || channelCount > 16)
        return stbi__errpuc("wrong channel count", "Unsupported number of channels in PSD image");

    // Read the rows and columns of the image.
    h = stbi__get32be(s);
    w = stbi__get32be(s);

    // Make sure the depth is 8 bits.
    bitdepth = stbi__get16be(s);
    if (bitdepth != 8 && bitdepth != 16)
        return stbi__errpuc("unsupported bit depth", "PSD bit depth is not 8 or 16 bit");

    // Make sure the color mode is RGB.
    // Valid options are:
    //   0: Bitmap
    //   1: Grayscale
    //   2: Indexed color
    //   3: RGB color
    //   4: CMYK color
    //   7: Multichannel
    //   8: Duotone
    //   9: Lab color
    if (stbi__get16be(s) != 3)
        return stbi__errpuc("wrong color format", "PSD is not in RGB color format");

    // Skip the Mode Data.  (It's the palette for indexed color; other info for other modes.)
    stbi__skip(s, stbi__get32be(s));

    // Skip the image resources.  (resolution, pen tool paths, etc)
    stbi__skip(s, stbi__get32be(s));

    // Skip the reserved data.
    stbi__skip(s, stbi__get32be(s));

    // Find out if the data is compressed.
    // Known values:
    //   0: no compression
    //   1: RLE compressed
    compression = stbi__get16be(s);
    if (compression > 1)
        return stbi__errpuc("bad compression", "PSD has an unknown compression format");

    // Check size
    if (!stbi__mad3sizes_valid(4, w, h, 0))
        return stbi__errpuc("too large", "Corrupt PSD");

    // Create the destination image.

    if (!compression && bitdepth == 16 && bpc == 16) {
        out = (stbi_uc*)stbi__malloc_mad3(8, w, h, 0);
        ri->bits_per_channel = 16;
    }
    else
        out = (stbi_uc*)stbi__malloc(4 * w * h);

    if (!out) return stbi__errpuc("outofmem", "Out of memory");
    pixelCount = w * h;

    // Initialize the data to zero.
    //memset( out, 0, pixelCount * 4 );

    // Finally, the image data.
    if (compression) {
        // RLE as used by .PSD and .TIFF
        // Loop until you get the number of unpacked bytes you are expecting:
        //     Read the next source byte into n.
        //     If n is between 0 and 127 inclusive, copy the next n+1 bytes literally.
        //     Else if n is between -127 and -1 inclusive, copy the next byte -n+1 times.
        //     Else if n is 128, noop.
        // Endloop

        // The RLE-compressed data is preceded by a 2-byte data count for each row in the data,
        // which we're going to just skip.
        stbi__skip(s, h * channelCount * 2);

        // Read the RLE data by channel.
        for (channel = 0; channel < 4; channel++) {
            stbi_uc* p;

            p = out + channel;
            if (channel >= channelCount) {
                // Fill this channel with default data.
                for (i = 0; i < pixelCount; i++, p += 4)
                    * p = (channel == 3 ? 255 : 0);
            }
            else {
                // Read the RLE data.
                if (!stbi__psd_decode_rle(s, p, pixelCount)) {
                    STBI_FREE(out);
                    return stbi__errpuc("corrupt", "bad RLE data");
                }
            }
        }

    }
    else {
        // We're at the raw image data.  It's each channel in order (Red, Green, Blue, Alpha, ...)
        // where each channel consists of an 8-bit (or 16-bit) value for each pixel in the image.

        // Read the data by channel.
        for (channel = 0; channel < 4; channel++) {
            if (channel >= channelCount) {
                // Fill this channel with default data.
                if (bitdepth == 16 && bpc == 16) {
                    stbi__uint16* q = ((stbi__uint16*)out) + channel;
                    stbi__uint16 val = channel == 3 ? 65535 : 0;
                    for (i = 0; i < pixelCount; i++, q += 4)
                        * q = val;
                }
                else {
                    stbi_uc* p = out + channel;
                    stbi_uc val = channel == 3 ? 255 : 0;
                    for (i = 0; i < pixelCount; i++, p += 4)
                        * p = val;
                }
            }
            else {
                if (ri->bits_per_channel == 16) {    // output bpc
                    stbi__uint16* q = ((stbi__uint16*)out) + channel;
                    for (i = 0; i < pixelCount; i++, q += 4)
                        * q = (stbi__uint16)stbi__get16be(s);
                }
                else {
                    stbi_uc* p = out + channel;
                    if (bitdepth == 16) {  // input bpc
                        for (i = 0; i < pixelCount; i++, p += 4)
                            * p = (stbi_uc)(stbi__get16be(s) >> 8);
                    }
                    else {
                        for (i = 0; i < pixelCount; i++, p += 4)
                            * p = stbi__get8(s);
                    }
                }
            }
        }
    }

    // remove weird white matte from PSD
    if (channelCount >= 4) {
        if (ri->bits_per_channel == 16) {
            for (i = 0; i < w * h; ++i) {
                stbi__uint16* pixel = (stbi__uint16*)out + 4 * i;
                if (pixel[3] != 0 && pixel[3] != 65535) {
                    float a = pixel[3] / 65535.0f;
                    float ra = 1.0f / a;
                    float inv_a = 65535.0f * (1 - ra);
                    pixel[0] = (stbi__uint16)(pixel[0] * ra + inv_a);
                    pixel[1] = (stbi__uint16)(pixel[1] * ra + inv_a);
                    pixel[2] = (stbi__uint16)(pixel[2] * ra + inv_a);
                }
            }
        }
        else {
            for (i = 0; i < w * h; ++i) {
                unsigned char* pixel = out + 4 * i;
                if (pixel[3] != 0 && pixel[3] != 255) {
                    float a = pixel[3] / 255.0f;
                    float ra = 1.0f / a;
                    float inv_a = 255.0f * (1 - ra);
                    pixel[0] = (unsigned char)(pixel[0] * ra + inv_a);
                    pixel[1] = (unsigned char)(pixel[1] * ra + inv_a);
                    pixel[2] = (unsigned char)(pixel[2] * ra + inv_a);
                }
            }
        }
    }

    // convert to desired output format
    if (req_comp && req_comp != 4) {
        if (ri->bits_per_channel == 16)
            out = (stbi_uc*)stbi__convert_format16((stbi__uint16*)out, 4, req_comp, w, h);
        else
            out = stbi__convert_format(out, 4, req_comp, w, h);
        if (out == NULL) return out; // stbi__convert_format frees input on failure
    }

    if (comp)* comp = 4;
    *y = h;
    *x = w;

    return out;
}
#endif

// *************************************************************************************************
// Softimage PIC loader
// by Tom Seddon
//
// See http://softimage.wiki.softimage.com/index.php/INFO:_PIC_file_format
// See http://ozviz.wasp.uwa.edu.au/~pbourke/dataformats/softimagepic/

#ifndef STBI_NO_PIC
static int stbi__pic_is4(stbi__context* s, const char* str)
{
    int i;
    for (i = 0; i < 4; ++i)
        if (stbi__get8(s) != (stbi_uc)str[i])
            return 0;

    return 1;
}

static int stbi__pic_test_core(stbi__context* s)
{
    int i;

    if (!stbi__pic_is4(s, "\x53\x80\xF6\x34"))
        return 0;

    for (i = 0; i < 84; ++i)
        stbi__get8(s);

    if (!stbi__pic_is4(s, "PICT"))
        return 0;

    return 1;
}

typedef struct
{
    stbi_uc size, type, channel;
} stbi__pic_packet;

static stbi_uc* stbi__readval(stbi__context* s, int channel, stbi_uc* dest)
{
    int mask = 0x80, i;

    for (i = 0; i < 4; ++i, mask >>= 1) {
        if (channel & mask) {
            if (stbi__at_eof(s)) return stbi__errpuc("bad file", "PIC file too short");
            dest[i] = stbi__get8(s);
        }
    }

    return dest;
}

static void stbi__copyval(int channel, stbi_uc* dest, const stbi_uc* src)
{
    int mask = 0x80, i;

    for (i = 0; i < 4; ++i, mask >>= 1)
        if (channel & mask)
            dest[i] = src[i];
}

static stbi_uc* stbi__pic_load_core(stbi__context* s, int width, int height, int* comp, stbi_uc* result)
{
    int act_comp = 0, num_packets = 0, y, chained;
    stbi__pic_packet packets[10];

    // this will (should...) cater for even some bizarre stuff like having data
     // for the same channel in multiple packets.
    do {
        stbi__pic_packet* packet;

        if (num_packets == sizeof(packets) / sizeof(packets[0]))
            return stbi__errpuc("bad format", "too many packets");

        packet = &packets[num_packets++];

        chained = stbi__get8(s);
        packet->size = stbi__get8(s);
        packet->type = stbi__get8(s);
        packet->channel = stbi__get8(s);

        act_comp |= packet->channel;

        if (stbi__at_eof(s))          return stbi__errpuc("bad file", "file too short (reading packets)");
        if (packet->size != 8)  return stbi__errpuc("bad format", "packet isn't 8bpp");
    } while (chained);

    *comp = (act_comp & 0x10 ? 4 : 3); // has alpha channel?

    for (y = 0; y < height; ++y) {
        int packet_idx;

        for (packet_idx = 0; packet_idx < num_packets; ++packet_idx) {
            stbi__pic_packet* packet = &packets[packet_idx];
            stbi_uc* dest = result + y * width * 4;

            switch (packet->type) {
            default:
                return stbi__errpuc("bad format", "packet has bad compression type");

            case 0: {//uncompressed
                int x;

                for (x = 0; x < width; ++x, dest += 4)
                    if (!stbi__readval(s, packet->channel, dest))
                        return 0;
                break;
            }

            case 1://Pure RLE
            {
                int left = width, i;

                while (left > 0) {
                    stbi_uc count, value[4];

                    count = stbi__get8(s);
                    if (stbi__at_eof(s))   return stbi__errpuc("bad file", "file too short (pure read count)");

                    if (count > left)
                        count = (stbi_uc)left;

                    if (!stbi__readval(s, packet->channel, value))  return 0;

                    for (i = 0; i < count; ++i, dest += 4)
                        stbi__copyval(packet->channel, dest, value);
                    left -= count;
                }
            }
            break;

            case 2: {//Mixed RLE
                int left = width;
                while (left > 0) {
                    int count = stbi__get8(s), i;
                    if (stbi__at_eof(s))  return stbi__errpuc("bad file", "file too short (mixed read count)");

                    if (count >= 128) { // Repeated
                        stbi_uc value[4];

                        if (count == 128)
                            count = stbi__get16be(s);
                        else
                            count -= 127;
                        if (count > left)
                            return stbi__errpuc("bad file", "scanline overrun");

                        if (!stbi__readval(s, packet->channel, value))
                            return 0;

                        for (i = 0; i < count; ++i, dest += 4)
                            stbi__copyval(packet->channel, dest, value);
                    }
                    else { // Raw
                        ++count;
                        if (count > left) return stbi__errpuc("bad file", "scanline overrun");

                        for (i = 0; i < count; ++i, dest += 4)
                            if (!stbi__readval(s, packet->channel, dest))
                                return 0;
                    }
                    left -= count;
                }
                break;
            }
            }
        }
    }

    return result;
}

static void* stbi__pic_load(stbi__context* s, int* px, int* py, int* comp, int req_comp, stbi__result_info* ri)
{
    stbi_uc* result;
    int i, x, y, internal_comp;
    STBI_NOTUSED(ri);

    if (!comp) comp = &internal_comp;

    for (i = 0; i < 92; ++i)
        stbi__get8(s);

    x = stbi__get16be(s);
    y = stbi__get16be(s);
    if (stbi__at_eof(s))  return stbi__errpuc("bad file", "file too short (pic header)");
    if (!stbi__mad3sizes_valid(x, y, 4, 0)) return stbi__errpuc("too large", "PIC image too large to decode");

    stbi__get32be(s); //skip `ratio'
    stbi__get16be(s); //skip `fields'
    stbi__get16be(s); //skip `pad'

    // intermediate buffer is RGBA
    result = (stbi_uc*)stbi__malloc_mad3(x, y, 4, 0);
    memset(result, 0xff, x * y * 4);

    if (!stbi__pic_load_core(s, x, y, comp, result)) {
        STBI_FREE(result);
        result = 0;
    }
    *px = x;
    *py = y;
    if (req_comp == 0) req_comp = *comp;
    result = stbi__convert_format(result, 4, req_comp, x, y);

    return result;
}

static int stbi__pic_test(stbi__context* s)
{
    int r = stbi__pic_test_core(s);
    stbi__rewind(s);
    return r;
}
#endif

// *************************************************************************************************
// GIF loader -- public domain by Jean-Marc Lienher -- simplified/shrunk by stb

#ifndef STBI_NO_GIF
typedef struct
{
    stbi__int16 prefix;
    stbi_uc first;
    stbi_uc suffix;
} stbi__gif_lzw;

typedef struct
{
    int w, h;
    stbi_uc* out;                 // output buffer (always 4 components)
    stbi_uc* background;          // The current "background" as far as a gif is concerned
    stbi_uc* history;
    int flags, bgindex, ratio, transparent, eflags;
    stbi_uc  pal[256][4];
    stbi_uc lpal[256][4];
    stbi__gif_lzw codes[8192];
    stbi_uc* color_table;
    int parse, step;
    int lflags;
    int start_x, start_y;
    int max_x, max_y;
    int cur_x, cur_y;
    int line_size;
    int delay;
} stbi__gif;

static int stbi__gif_test_raw(stbi__context* s)
{
    int sz;
    if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' || stbi__get8(s) != '8') return 0;
    sz = stbi__get8(s);
    if (sz != '9' && sz != '7') return 0;
    if (stbi__get8(s) != 'a') return 0;
    return 1;
}

static int stbi__gif_test(stbi__context* s)
{
    int r = stbi__gif_test_raw(s);
    stbi__rewind(s);
    return r;
}

static void stbi__gif_parse_colortable(stbi__context* s, stbi_uc pal[256][4], int num_entries, int transp)
{
    int i;
    for (i = 0; i < num_entries; ++i) {
        pal[i][2] = stbi__get8(s);
        pal[i][1] = stbi__get8(s);
        pal[i][0] = stbi__get8(s);
        pal[i][3] = transp == i ? 0 : 255;
    }
}

static int stbi__gif_header(stbi__context* s, stbi__gif* g, int* comp, int is_info)
{
    stbi_uc version;
    if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' || stbi__get8(s) != '8')
        return stbi__err("not GIF", "Corrupt GIF");

    version = stbi__get8(s);
    if (version != '7' && version != '9')    return stbi__err("not GIF", "Corrupt GIF");
    if (stbi__get8(s) != 'a')                return stbi__err("not GIF", "Corrupt GIF");

    stbi__g_failure_reason = "";
    g->w = stbi__get16le(s);
    g->h = stbi__get16le(s);
    g->flags = stbi__get8(s);
    g->bgindex = stbi__get8(s);
    g->ratio = stbi__get8(s);
    g->transparent = -1;

    if (comp != 0)* comp = 4;  // can't actually tell whether it's 3 or 4 until we parse the comments

    if (is_info) return 1;

    if (g->flags & 0x80)
        stbi__gif_parse_colortable(s, g->pal, 2 << (g->flags & 7), -1);

    return 1;
}

static int stbi__gif_info_raw(stbi__context* s, int* x, int* y, int* comp)
{
    stbi__gif* g = (stbi__gif*)stbi__malloc(sizeof(stbi__gif));
    if (!stbi__gif_header(s, g, comp, 1)) {
        STBI_FREE(g);
        stbi__rewind(s);
        return 0;
    }
    if (x)* x = g->w;
    if (y)* y = g->h;
    STBI_FREE(g);
    return 1;
}

static void stbi__out_gif_code(stbi__gif* g, stbi__uint16 code)
{
    stbi_uc* p, * c;
    int idx;

    // recurse to decode the prefixes, since the linked-list is backwards,
    // and working backwards through an interleaved image would be nasty
    if (g->codes[code].prefix >= 0)
        stbi__out_gif_code(g, g->codes[code].prefix);

    if (g->cur_y >= g->max_y) return;

    idx = g->cur_x + g->cur_y;
    p = &g->out[idx];
    g->history[idx / 4] = 1;

    c = &g->color_table[g->codes[code].suffix * 4];
    if (c[3] > 128) { // don't render transparent pixels; 
        p[0] = c[2];
        p[1] = c[1];
        p[2] = c[0];
        p[3] = c[3];
    }
    g->cur_x += 4;

    if (g->cur_x >= g->max_x) {
        g->cur_x = g->start_x;
        g->cur_y += g->step;

        while (g->cur_y >= g->max_y && g->parse > 0) {
            g->step = (1 << g->parse) * g->line_size;
            g->cur_y = g->start_y + (g->step >> 1);
            --g->parse;
        }
    }
}

static stbi_uc* stbi__process_gif_raster(stbi__context* s, stbi__gif* g)
{
    stbi_uc lzw_cs;
    stbi__int32 len, init_code;
    stbi__uint32 first;
    stbi__int32 codesize, codemask, avail, oldcode, bits, valid_bits, clear;
    stbi__gif_lzw* p;

    lzw_cs = stbi__get8(s);
    if (lzw_cs > 12) return NULL;
    clear = 1 << lzw_cs;
    first = 1;
    codesize = lzw_cs + 1;
    codemask = (1 << codesize) - 1;
    bits = 0;
    valid_bits = 0;
    for (init_code = 0; init_code < clear; init_code++) {
        g->codes[init_code].prefix = -1;
        g->codes[init_code].first = (stbi_uc)init_code;
        g->codes[init_code].suffix = (stbi_uc)init_code;
    }

    // support no starting clear code
    avail = clear + 2;
    oldcode = -1;

    len = 0;
    for (;;) {
        if (valid_bits < codesize) {
            if (len == 0) {
                len = stbi__get8(s); // start new block
                if (len == 0)
                    return g->out;
            }
            --len;
            bits |= (stbi__int32)stbi__get8(s) << valid_bits;
            valid_bits += 8;
        }
        else {
            stbi__int32 code = bits & codemask;
            bits >>= codesize;
            valid_bits -= codesize;
            // @OPTIMIZE: is there some way we can accelerate the non-clear path?
            if (code == clear) {  // clear code
                codesize = lzw_cs + 1;
                codemask = (1 << codesize) - 1;
                avail = clear + 2;
                oldcode = -1;
                first = 0;
            }
            else if (code == clear + 1) { // end of stream code
                stbi__skip(s, len);
                while ((len = stbi__get8(s)) > 0)
                    stbi__skip(s, len);
                return g->out;
            }
            else if (code <= avail) {
                if (first) {
                    return stbi__errpuc("no clear code", "Corrupt GIF");
                }

                if (oldcode >= 0) {
                    p = &g->codes[avail++];
                    if (avail > 8192) {
                        return stbi__errpuc("too many codes", "Corrupt GIF");
                    }

                    p->prefix = (stbi__int16)oldcode;
                    p->first = g->codes[oldcode].first;
                    p->suffix = (code == avail) ? p->first : g->codes[code].first;
                }
                else if (code == avail)
                    return stbi__errpuc("illegal code in raster", "Corrupt GIF");

                stbi__out_gif_code(g, (stbi__uint16)code);

                if ((avail & codemask) == 0 && avail <= 0x0FFF) {
                    codesize++;
                    codemask = (1 << codesize) - 1;
                }

                oldcode = code;
            }
            else {
                return stbi__errpuc("illegal code in raster", "Corrupt GIF");
            }
        }
    }
}

// this function is designed to support animated gifs, although stb_image doesn't support it
// two back is the image from two frames ago, used for a very specific disposal format
static stbi_uc* stbi__gif_load_next(stbi__context* s, stbi__gif* g, int* comp, int req_comp, stbi_uc* two_back)
{
    int dispose;
    int first_frame;
    int pi;
    int pcount;
    STBI_NOTUSED(req_comp);

    // on first frame, any non-written pixels get the background colour (non-transparent)
    first_frame = 0;
    if (g->out == 0) {
        if (!stbi__gif_header(s, g, comp, 0)) return 0; // stbi__g_failure_reason set by stbi__gif_header
        if (!stbi__mad3sizes_valid(4, g->w, g->h, 0))
            return stbi__errpuc("too large", "GIF image is too large");
        pcount = g->w * g->h;
        g->out = (stbi_uc*)stbi__malloc(4 * pcount);
        g->background = (stbi_uc*)stbi__malloc(4 * pcount);
        g->history = (stbi_uc*)stbi__malloc(pcount);
        if (!g->out || !g->background || !g->history)
            return stbi__errpuc("outofmem", "Out of memory");

        // image is treated as "transparent" at the start - ie, nothing overwrites the current background; 
        // background colour is only used for pixels that are not rendered first frame, after that "background"
        // color refers to the color that was there the previous frame. 
        memset(g->out, 0x00, 4 * pcount);
        memset(g->background, 0x00, 4 * pcount); // state of the background (starts transparent)
        memset(g->history, 0x00, pcount);        // pixels that were affected previous frame
        first_frame = 1;
    }
    else {
        // second frame - how do we dispoase of the previous one?
        dispose = (g->eflags & 0x1C) >> 2;
        pcount = g->w * g->h;

        if ((dispose == 3) && (two_back == 0)) {
            dispose = 2; // if I don't have an image to revert back to, default to the old background
        }

        if (dispose == 3) { // use previous graphic
            for (pi = 0; pi < pcount; ++pi) {
                if (g->history[pi]) {
                    memcpy(&g->out[pi * 4], &two_back[pi * 4], 4);
                }
            }
        }
        else if (dispose == 2) {
            // restore what was changed last frame to background before that frame; 
            for (pi = 0; pi < pcount; ++pi) {
                if (g->history[pi]) {
                    memcpy(&g->out[pi * 4], &g->background[pi * 4], 4);
                }
            }
        }
        else {
            // This is a non-disposal case eithe way, so just 
            // leave the pixels as is, and they will become the new background
            // 1: do not dispose
            // 0:  not specified.
        }

        // background is what out is after the undoing of the previou frame; 
        memcpy(g->background, g->out, 4 * g->w * g->h);
    }

    // clear my history; 
    memset(g->history, 0x00, g->w * g->h);        // pixels that were affected previous frame

    for (;;) {
        int tag = stbi__get8(s);
        switch (tag) {
        case 0x2C: /* Image Descriptor */
        {
            stbi__int32 x, y, w, h;
            stbi_uc* o;

            x = stbi__get16le(s);
            y = stbi__get16le(s);
            w = stbi__get16le(s);
            h = stbi__get16le(s);
            if (((x + w) > (g->w)) || ((y + h) > (g->h)))
                return stbi__errpuc("bad Image Descriptor", "Corrupt GIF");

            g->line_size = g->w * 4;
            g->start_x = x * 4;
            g->start_y = y * g->line_size;
            g->max_x = g->start_x + w * 4;
            g->max_y = g->start_y + h * g->line_size;
            g->cur_x = g->start_x;
            g->cur_y = g->start_y;

            // if the width of the specified rectangle is 0, that means
            // we may not see *any* pixels or the image is malformed;
            // to make sure this is caught, move the current y down to
            // max_y (which is what out_gif_code checks).
            if (w == 0)
                g->cur_y = g->max_y;

            g->lflags = stbi__get8(s);

            if (g->lflags & 0x40) {
                g->step = 8 * g->line_size; // first interlaced spacing
                g->parse = 3;
            }
            else {
                g->step = g->line_size;
                g->parse = 0;
            }

            if (g->lflags & 0x80) {
                stbi__gif_parse_colortable(s, g->lpal, 2 << (g->lflags & 7), g->eflags & 0x01 ? g->transparent : -1);
                g->color_table = (stbi_uc*)g->lpal;
            }
            else if (g->flags & 0x80) {
                g->color_table = (stbi_uc*)g->pal;
            }
            else
                return stbi__errpuc("missing color table", "Corrupt GIF");

            o = stbi__process_gif_raster(s, g);
            if (!o) return NULL;

            // if this was the first frame, 
            pcount = g->w * g->h;
            if (first_frame && (g->bgindex > 0)) {
                // if first frame, any pixel not drawn to gets the background color
                for (pi = 0; pi < pcount; ++pi) {
                    if (g->history[pi] == 0) {
                        g->pal[g->bgindex][3] = 255; // just in case it was made transparent, undo that; It will be reset next frame if need be; 
                        memcpy(&g->out[pi * 4], &g->pal[g->bgindex], 4);
                    }
                }
            }

            return o;
        }

        case 0x21: // Comment Extension.
        {
            int len;
            int ext = stbi__get8(s);
            if (ext == 0xF9) { // Graphic Control Extension.
                len = stbi__get8(s);
                if (len == 4) {
                    g->eflags = stbi__get8(s);
                    g->delay = 10 * stbi__get16le(s); // delay - 1/100th of a second, saving as 1/1000ths.

                    // unset old transparent
                    if (g->transparent >= 0) {
                        g->pal[g->transparent][3] = 255;
                    }
                    if (g->eflags & 0x01) {
                        g->transparent = stbi__get8(s);
                        if (g->transparent >= 0) {
                            g->pal[g->transparent][3] = 0;
                        }
                    }
                    else {
                        // don't need transparent
                        stbi__skip(s, 1);
                        g->transparent = -1;
                    }
                }
                else {
                    stbi__skip(s, len);
                    break;
                }
            }
            while ((len = stbi__get8(s)) != 0) {
                stbi__skip(s, len);
            }
            break;
        }

        case 0x3B: // gif stream termination code
            return (stbi_uc*)s; // using '1' causes warning on some compilers

        default:
            return stbi__errpuc("unknown code", "Corrupt GIF");
        }
    }
}

static void* stbi__load_gif_main(stbi__context* s, int** delays, int* x, int* y, int* z, int* comp, int req_comp)
{
    if (stbi__gif_test(s)) {
        int layers = 0;
        stbi_uc* u = 0;
        stbi_uc* out = 0;
        stbi_uc* two_back = 0;
        stbi__gif g;
        int stride;
        memset(&g, 0, sizeof(g));
        if (delays) {
            *delays = 0;
        }

        do {
            u = stbi__gif_load_next(s, &g, comp, req_comp, two_back);
            if (u == (stbi_uc*)s) u = 0;  // end of animated gif marker

            if (u) {
                *x = g.w;
                *y = g.h;
                ++layers;
                stride = g.w * g.h * 4;

                if (out) {
                    out = (stbi_uc*)STBI_REALLOC(out, layers * stride);
                    if (delays) {
                        *delays = (int*)STBI_REALLOC(*delays, sizeof(int) * layers);
                    }
                }
                else {
                    out = (stbi_uc*)stbi__malloc(layers * stride);
                    if (delays) {
                        *delays = (int*)stbi__malloc(layers * sizeof(int));
                    }
                }
                memcpy(out + ((layers - 1) * stride), u, stride);
                if (layers >= 2) {
                    two_back = out - 2 * stride;
                }

                if (delays) {
                    (*delays)[layers - 1U] = g.delay;
                }
            }
        } while (u != 0);

        // free temp buffer; 
        STBI_FREE(g.out);
        STBI_FREE(g.history);
        STBI_FREE(g.background);

        // do the final conversion after loading everything; 
        if (req_comp && req_comp != 4)
            out = stbi__convert_format(out, 4, req_comp, layers * g.w, g.h);

        *z = layers;
        return out;
    }
    else {
        return stbi__errpuc("not GIF", "Image was not as a gif type.");
    }
}

static void* stbi__gif_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri)
{
    stbi_uc* u = 0;
    stbi__gif g;
    memset(&g, 0, sizeof(g));
    STBI_NOTUSED(ri);

    u = stbi__gif_load_next(s, &g, comp, req_comp, 0);
    if (u == (stbi_uc*)s) u = 0;  // end of animated gif marker
    if (u) {
        *x = g.w;
        *y = g.h;

        // moved conversion to after successful load so that the same
        // can be done for multiple frames. 
        if (req_comp && req_comp != 4)
            u = stbi__convert_format(u, 4, req_comp, g.w, g.h);
    }
    else if (g.out) {
        // if there was an error and we allocated an image buffer, free it!
        STBI_FREE(g.out);
    }

    // free buffers needed for multiple frame loading; 
    STBI_FREE(g.history);
    STBI_FREE(g.background);

    return u;
}

static int stbi__gif_info(stbi__context* s, int* x, int* y, int* comp)
{
    return stbi__gif_info_raw(s, x, y, comp);
}
#endif

// *************************************************************************************************
// Radiance RGBE HDR loader
// originally by Nicolas Schulz
#ifndef STBI_NO_HDR
static int stbi__hdr_test_core(stbi__context* s, const char* signature)
{
    int i;
    for (i = 0; signature[i]; ++i)
        if (stbi__get8(s) != signature[i])
            return 0;
    stbi__rewind(s);
    return 1;
}

static int stbi__hdr_test(stbi__context* s)
{
    int r = stbi__hdr_test_core(s, "#?RADIANCE\n");
    stbi__rewind(s);
    if (!r) {
        r = stbi__hdr_test_core(s, "#?RGBE\n");
        stbi__rewind(s);
    }
    return r;
}

#define STBI__HDR_BUFLEN  1024
static char* stbi__hdr_gettoken(stbi__context * z, char* buffer)
{
    int len = 0;
    char c = '\0';

    c = (char)stbi__get8(z);

    while (!stbi__at_eof(z) && c != '\n') {
        buffer[len++] = c;
        if (len == STBI__HDR_BUFLEN - 1) {
            // flush to end of line
            while (!stbi__at_eof(z) && stbi__get8(z) != '\n')
                ;
            break;
        }
        c = (char)stbi__get8(z);
    }

    buffer[len] = 0;
    return buffer;
}

static void stbi__hdr_convert(float* output, stbi_uc* input, int req_comp)
{
    if (input[3] != 0) {
        float f1;
        // Exponent
        f1 = (float)ldexp(1.0f, input[3] - (int)(128 + 8));
        if (req_comp <= 2)
            output[0] = (input[0] + input[1] + input[2]) * f1 / 3;
        else {
            output[0] = input[0] * f1;
            output[1] = input[1] * f1;
            output[2] = input[2] * f1;
        }
        if (req_comp == 2) output[1] = 1;
        if (req_comp == 4) output[3] = 1;
    }
    else {
        switch (req_comp) {
        case 4: output[3] = 1; /* fallthrough */
        case 3: output[0] = output[1] = output[2] = 0;
            break;
        case 2: output[1] = 1; /* fallthrough */
        case 1: output[0] = 0;
            break;
        }
    }
}

static float* stbi__hdr_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri)
{
    char buffer[STBI__HDR_BUFLEN];
    char* token;
    int valid = 0;
    int width, height;
    stbi_uc* scanline;
    float* hdr_data;
    int len;
    unsigned char count, value;
    int i, j, k, c1, c2, z;
    const char* headerToken;
    STBI_NOTUSED(ri);

    // Check identifier
    headerToken = stbi__hdr_gettoken(s, buffer);
    if (strcmp(headerToken, "#?RADIANCE") != 0 && strcmp(headerToken, "#?RGBE") != 0)
        return stbi__errpf("not HDR", "Corrupt HDR image");

    // Parse header
    for (;;) {
        token = stbi__hdr_gettoken(s, buffer);
        if (token[0] == 0) break;
        if (strcmp(token, "FORMAT=32-bit_rle_rgbe") == 0) valid = 1;
    }

    if (!valid)    return stbi__errpf("unsupported format", "Unsupported HDR format");

    // Parse width and height
    // can't use sscanf() if we're not using stdio!
    token = stbi__hdr_gettoken(s, buffer);
    if (strncmp(token, "-Y ", 3))  return stbi__errpf("unsupported data layout", "Unsupported HDR format");
    token += 3;
    height = (int)strtol(token, &token, 10);
    while (*token == ' ') ++token;
    if (strncmp(token, "+X ", 3))  return stbi__errpf("unsupported data layout", "Unsupported HDR format");
    token += 3;
    width = (int)strtol(token, NULL, 10);

    *x = width;
    *y = height;

    if (comp)* comp = 3;
    if (req_comp == 0) req_comp = 3;

    if (!stbi__mad4sizes_valid(width, height, req_comp, sizeof(float), 0))
        return stbi__errpf("too large", "HDR image is too large");

    // Read data
    hdr_data = (float*)stbi__malloc_mad4(width, height, req_comp, sizeof(float), 0);
    if (!hdr_data)
        return stbi__errpf("outofmem", "Out of memory");

    // Load image data
    // image data is stored as some number of sca
    if (width < 8 || width >= 32768) {
        // Read flat data
        for (j = 0; j < height; ++j) {
            for (i = 0; i < width; ++i) {
                stbi_uc rgbe[4];
            main_decode_loop:
                stbi__getn(s, rgbe, 4);
                stbi__hdr_convert(hdr_data + j * width * req_comp + i * req_comp, rgbe, req_comp);
            }
        }
    }
    else {
        // Read RLE-encoded data
        scanline = NULL;

        for (j = 0; j < height; ++j) {
            c1 = stbi__get8(s);
            c2 = stbi__get8(s);
            len = stbi__get8(s);
            if (c1 != 2 || c2 != 2 || (len & 0x80)) {
                // not run-length encoded, so we have to actually use THIS data as a decoded
                // pixel (note this can't be a valid pixel--one of RGB must be >= 128)
                stbi_uc rgbe[4];
                rgbe[0] = (stbi_uc)c1;
                rgbe[1] = (stbi_uc)c2;
                rgbe[2] = (stbi_uc)len;
                rgbe[3] = (stbi_uc)stbi__get8(s);
                stbi__hdr_convert(hdr_data, rgbe, req_comp);
                i = 1;
                j = 0;
                STBI_FREE(scanline);
                goto main_decode_loop; // yes, this makes no sense
            }
            len <<= 8;
            len |= stbi__get8(s);
            if (len != width) { STBI_FREE(hdr_data); STBI_FREE(scanline); return stbi__errpf("invalid decoded scanline length", "corrupt HDR"); }
            if (scanline == NULL) {
                scanline = (stbi_uc*)stbi__malloc_mad2(width, 4, 0);
                if (!scanline) {
                    STBI_FREE(hdr_data);
                    return stbi__errpf("outofmem", "Out of memory");
                }
            }

            for (k = 0; k < 4; ++k) {
                int nleft;
                i = 0;
                while ((nleft = width - i) > 0) {
                    count = stbi__get8(s);
                    if (count > 128) {
                        // Run
                        value = stbi__get8(s);
                        count -= 128;
                        if (count > nleft) { STBI_FREE(hdr_data); STBI_FREE(scanline); return stbi__errpf("corrupt", "bad RLE data in HDR"); }
                        for (z = 0; z < count; ++z)
                            scanline[i++ * 4 + k] = value;
                    }
                    else {
                        // Dump
                        if (count > nleft) { STBI_FREE(hdr_data); STBI_FREE(scanline); return stbi__errpf("corrupt", "bad RLE data in HDR"); }
                        for (z = 0; z < count; ++z)
                            scanline[i++ * 4 + k] = stbi__get8(s);
                    }
                }
            }
            for (i = 0; i < width; ++i)
                stbi__hdr_convert(hdr_data + (j * width + i) * req_comp, scanline + i * 4, req_comp);
        }
        if (scanline)
            STBI_FREE(scanline);
    }

    return hdr_data;
}

static int stbi__hdr_info(stbi__context* s, int* x, int* y, int* comp)
{
    char buffer[STBI__HDR_BUFLEN];
    char* token;
    int valid = 0;
    int dummy;

    if (!x) x = &dummy;
    if (!y) y = &dummy;
    if (!comp) comp = &dummy;

    if (stbi__hdr_test(s) == 0) {
        stbi__rewind(s);
        return 0;
    }

    for (;;) {
        token = stbi__hdr_gettoken(s, buffer);
        if (token[0] == 0) break;
        if (strcmp(token, "FORMAT=32-bit_rle_rgbe") == 0) valid = 1;
    }

    if (!valid) {
        stbi__rewind(s);
        return 0;
    }
    token = stbi__hdr_gettoken(s, buffer);
    if (strncmp(token, "-Y ", 3)) {
        stbi__rewind(s);
        return 0;
    }
    token += 3;
    *y = (int)strtol(token, &token, 10);
    while (*token == ' ') ++token;
    if (strncmp(token, "+X ", 3)) {
        stbi__rewind(s);
        return 0;
    }
    token += 3;
    *x = (int)strtol(token, NULL, 10);
    *comp = 3;
    return 1;
}
#endif // STBI_NO_HDR

#ifndef STBI_NO_BMP
static int stbi__bmp_info(stbi__context* s, int* x, int* y, int* comp)
{
    void* p;
    stbi__bmp_data info;

    info.all_a = 255;
    p = stbi__bmp_parse_header(s, &info);
    stbi__rewind(s);
    if (p == NULL)
        return 0;
    if (x)* x = s->img_x;
    if (y)* y = s->img_y;
    if (comp)* comp = info.ma ? 4 : 3;
    return 1;
}
#endif

#ifndef STBI_NO_PSD
static int stbi__psd_info(stbi__context* s, int* x, int* y, int* comp)
{
    int channelCount, dummy, depth;
    if (!x) x = &dummy;
    if (!y) y = &dummy;
    if (!comp) comp = &dummy;
    if (stbi__get32be(s) != 0x38425053) {
        stbi__rewind(s);
        return 0;
    }
    if (stbi__get16be(s) != 1) {
        stbi__rewind(s);
        return 0;
    }
    stbi__skip(s, 6);
    channelCount = stbi__get16be(s);
    if (channelCount < 0 || channelCount > 16) {
        stbi__rewind(s);
        return 0;
    }
    *y = stbi__get32be(s);
    *x = stbi__get32be(s);
    depth = stbi__get16be(s);
    if (depth != 8 && depth != 16) {
        stbi__rewind(s);
        return 0;
    }
    if (stbi__get16be(s) != 3) {
        stbi__rewind(s);
        return 0;
    }
    *comp = 4;
    return 1;
}

static int stbi__psd_is16(stbi__context* s)
{
    int channelCount, depth;
    if (stbi__get32be(s) != 0x38425053) {
        stbi__rewind(s);
        return 0;
    }
    if (stbi__get16be(s) != 1) {
        stbi__rewind(s);
        return 0;
    }
    stbi__skip(s, 6);
    channelCount = stbi__get16be(s);
    if (channelCount < 0 || channelCount > 16) {
        stbi__rewind(s);
        return 0;
    }
    (void)stbi__get32be(s);
    (void)stbi__get32be(s);
    depth = stbi__get16be(s);
    if (depth != 16) {
        stbi__rewind(s);
        return 0;
    }
    return 1;
}
#endif

#ifndef STBI_NO_PIC
static int stbi__pic_info(stbi__context* s, int* x, int* y, int* comp)
{
    int act_comp = 0, num_packets = 0, chained, dummy;
    stbi__pic_packet packets[10];

    if (!x) x = &dummy;
    if (!y) y = &dummy;
    if (!comp) comp = &dummy;

    if (!stbi__pic_is4(s, "\x53\x80\xF6\x34")) {
        stbi__rewind(s);
        return 0;
    }

    stbi__skip(s, 88);

    *x = stbi__get16be(s);
    *y = stbi__get16be(s);
    if (stbi__at_eof(s)) {
        stbi__rewind(s);
        return 0;
    }
    if ((*x) != 0 && (1 << 28) / (*x) < (*y)) {
        stbi__rewind(s);
        return 0;
    }

    stbi__skip(s, 8);

    do {
        stbi__pic_packet* packet;

        if (num_packets == sizeof(packets) / sizeof(packets[0]))
            return 0;

        packet = &packets[num_packets++];
        chained = stbi__get8(s);
        packet->size = stbi__get8(s);
        packet->type = stbi__get8(s);
        packet->channel = stbi__get8(s);
        act_comp |= packet->channel;

        if (stbi__at_eof(s)) {
            stbi__rewind(s);
            return 0;
        }
        if (packet->size != 8) {
            stbi__rewind(s);
            return 0;
        }
    } while (chained);

    *comp = (act_comp & 0x10 ? 4 : 3);

    return 1;
}
#endif

// *************************************************************************************************
// Portable Gray Map and Portable Pixel Map loader
// by Ken Miller
//
// PGM: http://netpbm.sourceforge.net/doc/pgm.html
// PPM: http://netpbm.sourceforge.net/doc/ppm.html
//
// Known limitations:
//    Does not support comments in the header section
//    Does not support ASCII image data (formats P2 and P3)
//    Does not support 16-bit-per-channel

#ifndef STBI_NO_PNM

static int      stbi__pnm_test(stbi__context* s)
{
    char p, t;
    p = (char)stbi__get8(s);
    t = (char)stbi__get8(s);
    if (p != 'P' || (t != '5' && t != '6')) {
        stbi__rewind(s);
        return 0;
    }
    return 1;
}

static void* stbi__pnm_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri)
{
    stbi_uc* out;
    STBI_NOTUSED(ri);

    if (!stbi__pnm_info(s, (int*)& s->img_x, (int*)& s->img_y, (int*)& s->img_n))
        return 0;

    *x = s->img_x;
    *y = s->img_y;
    if (comp)* comp = s->img_n;

    if (!stbi__mad3sizes_valid(s->img_n, s->img_x, s->img_y, 0))
        return stbi__errpuc("too large", "PNM too large");

    out = (stbi_uc*)stbi__malloc_mad3(s->img_n, s->img_x, s->img_y, 0);
    if (!out) return stbi__errpuc("outofmem", "Out of memory");
    stbi__getn(s, out, s->img_n * s->img_x * s->img_y);

    if (req_comp && req_comp != s->img_n) {
        out = stbi__convert_format(out, s->img_n, req_comp, s->img_x, s->img_y);
        if (out == NULL) return out; // stbi__convert_format frees input on failure
    }
    return out;
}

static int      stbi__pnm_isspace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

static void     stbi__pnm_skip_whitespace(stbi__context* s, char* c)
{
    for (;;) {
        while (!stbi__at_eof(s) && stbi__pnm_isspace(*c))
            * c = (char)stbi__get8(s);

        if (stbi__at_eof(s) || *c != '#')
            break;

        while (!stbi__at_eof(s) && *c != '\n' && *c != '\r')
            * c = (char)stbi__get8(s);
    }
}

static int      stbi__pnm_isdigit(char c)
{
    return c >= '0' && c <= '9';
}

static int      stbi__pnm_getinteger(stbi__context* s, char* c)
{
    int value = 0;

    while (!stbi__at_eof(s) && stbi__pnm_isdigit(*c)) {
        value = value * 10 + (*c - '0');
        *c = (char)stbi__get8(s);
    }

    return value;
}

static int      stbi__pnm_info(stbi__context* s, int* x, int* y, int* comp)
{
    int maxv, dummy;
    char c, p, t;

    if (!x) x = &dummy;
    if (!y) y = &dummy;
    if (!comp) comp = &dummy;

    stbi__rewind(s);

    // Get identifier
    p = (char)stbi__get8(s);
    t = (char)stbi__get8(s);
    if (p != 'P' || (t != '5' && t != '6')) {
        stbi__rewind(s);
        return 0;
    }

    *comp = (t == '6') ? 3 : 1;  // '5' is 1-component .pgm; '6' is 3-component .ppm

    c = (char)stbi__get8(s);
    stbi__pnm_skip_whitespace(s, &c);

    *x = stbi__pnm_getinteger(s, &c); // read width
    stbi__pnm_skip_whitespace(s, &c);

    *y = stbi__pnm_getinteger(s, &c); // read height
    stbi__pnm_skip_whitespace(s, &c);

    maxv = stbi__pnm_getinteger(s, &c);  // read max value

    if (maxv > 255)
        return stbi__err("max value > 255", "PPM image not 8-bit");
    else
        return 1;
}
#endif

static int stbi__info_main(stbi__context* s, int* x, int* y, int* comp)
{
#ifndef STBI_NO_JPEG
    if (stbi__jpeg_info(s, x, y, comp)) return 1;
#endif

#ifndef STBI_NO_PNG
    if (stbi__png_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_GIF
    if (stbi__gif_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_BMP
    if (stbi__bmp_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_PSD
    if (stbi__psd_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_PIC
    if (stbi__pic_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_PNM
    if (stbi__pnm_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_HDR
    if (stbi__hdr_info(s, x, y, comp))  return 1;
#endif

    // test tga last because it's a crappy test!
#ifndef STBI_NO_TGA
    if (stbi__tga_info(s, x, y, comp))
        return 1;
#endif
    return stbi__err("unknown image type", "Image not of any known type, or corrupt");
}

static int stbi__is_16_main(stbi__context* s)
{
#ifndef STBI_NO_PNG
    if (stbi__png_is16(s))  return 1;
#endif

#ifndef STBI_NO_PSD
    if (stbi__psd_is16(s))  return 1;
#endif

    return 0;
}

#ifndef STBI_NO_STDIO
STBIDEF int stbi_info(char const* filename, int* x, int* y, int* comp)
{
    FILE* f = stbi__fopen(filename, "rb");
    int result;
    if (!f) return stbi__err("can't fopen", "Unable to open file");
    result = stbi_info_from_file(f, x, y, comp);
    fclose(f);
    return result;
}

STBIDEF int stbi_info_from_file(FILE* f, int* x, int* y, int* comp)
{
    int r;
    stbi__context s;
    long pos = ftell(f);
    stbi__start_file(&s, f);
    r = stbi__info_main(&s, x, y, comp);
    fseek(f, pos, SEEK_SET);
    return r;
}

STBIDEF int stbi_is_16_bit(char const* filename)
{
    FILE* f = stbi__fopen(filename, "rb");
    int result;
    if (!f) return stbi__err("can't fopen", "Unable to open file");
    result = stbi_is_16_bit_from_file(f);
    fclose(f);
    return result;
}

STBIDEF int stbi_is_16_bit_from_file(FILE* f)
{
    int r;
    stbi__context s;
    long pos = ftell(f);
    stbi__start_file(&s, f);
    r = stbi__is_16_main(&s);
    fseek(f, pos, SEEK_SET);
    return r;
}
#endif // !STBI_NO_STDIO

STBIDEF int stbi_info_from_memory(stbi_uc const* buffer, int len, int* x, int* y, int* comp)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__info_main(&s, x, y, comp);
}

STBIDEF int stbi_info_from_callbacks(stbi_io_callbacks const* c, void* user, int* x, int* y, int* comp)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks*)c, user);
    return stbi__info_main(&s, x, y, comp);
}

STBIDEF int stbi_is_16_bit_from_memory(stbi_uc const* buffer, int len)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__is_16_main(&s);
}

STBIDEF int stbi_is_16_bit_from_callbacks(stbi_io_callbacks const* c, void* user)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks*)c, user);
    return stbi__is_16_main(&s);
}

#endif // STB_IMAGE_IMPLEMENTATION

/*
   revision history:
      2.20  (2019-02-07) support utf8 filenames in Windows; fix warnings and platform ifdefs
      2.19  (2018-02-11) fix warning
      2.18  (2018-01-30) fix warnings
      2.17  (2018-01-29) change sbti__shiftsigned to avoid clang -O2 bug
                         1-bit BMP
                         *_is_16_bit api
                         avoid warnings
      2.16  (2017-07-23) all functions have 16-bit variants;
                         STBI_NO_STDIO works again;
                         compilation fixes;
                         fix rounding in unpremultiply;
                         optimize vertical flip;
                         disable raw_len validation;
                         documentation fixes
      2.15  (2017-03-18) fix png-1,2,4 bug; now all Imagenet JPGs decode;
                         warning fixes; disable run-time SSE detection on gcc;
                         uniform handling of optional "return" values;
                         thread-safe initialization of zlib tables
      2.14  (2017-03-03) remove deprecated STBI_JPEG_OLD; fixes for Imagenet JPGs
      2.13  (2016-11-29) add 16-bit API, only supported for PNG right now
      2.12  (2016-04-02) fix typo in 2.11 PSD fix that caused crashes
      2.11  (2016-04-02) allocate large structures on the stack
                         remove white matting for transparent PSD
                         fix reported channel count for PNG & BMP
                         re-enable SSE2 in non-gcc 64-bit
                         support RGB-formatted JPEG
                         read 16-bit PNGs (only as 8-bit)
      2.10  (2016-01-22) avoid warning introduced in 2.09 by STBI_REALLOC_SIZED
      2.09  (2016-01-16) allow comments in PNM files
                         16-bit-per-pixel TGA (not bit-per-component)
                         info() for TGA could break due to .hdr handling
                         info() for BMP to shares code instead of sloppy parse
                         can use STBI_REALLOC_SIZED if allocator doesn't support realloc
                         code cleanup
      2.08  (2015-09-13) fix to 2.07 cleanup, reading RGB PSD as RGBA
      2.07  (2015-09-13) fix compiler warnings
                         partial animated GIF support
                         limited 16-bpc PSD support
                         #ifdef unused functions
                         bug with < 92 byte PIC,PNM,HDR,TGA
      2.06  (2015-04-19) fix bug where PSD returns wrong '*comp' value
      2.05  (2015-04-19) fix bug in progressive JPEG handling, fix warning
      2.04  (2015-04-15) try to re-enable SIMD on MinGW 64-bit
      2.03  (2015-04-12) extra corruption checking (mmozeiko)
                         stbi_set_flip_vertically_on_load (nguillemot)
                         fix NEON support; fix mingw support
      2.02  (2015-01-19) fix incorrect assert, fix warning
      2.01  (2015-01-17) fix various warnings; suppress SIMD on gcc 32-bit without -msse2
      2.00b (2014-12-25) fix STBI_MALLOC in progressive JPEG
      2.00  (2014-12-25) optimize JPG, including x86 SSE2 & NEON SIMD (ryg)
                         progressive JPEG (stb)
                         PGM/PPM support (Ken Miller)
                         STBI_MALLOC,STBI_REALLOC,STBI_FREE
                         GIF bugfix -- seemingly never worked
                         STBI_NO_*, STBI_ONLY_*
      1.48  (2014-12-14) fix incorrectly-named assert()
      1.47  (2014-12-14) 1/2/4-bit PNG support, both direct and paletted (Omar Cornut & stb)
                         optimize PNG (ryg)
                         fix bug in interlaced PNG with user-specified channel count (stb)
      1.46  (2014-08-26)
              fix broken tRNS chunk (colorkey-style transparency) in non-paletted PNG
      1.45  (2014-08-16)
              fix MSVC-ARM internal compiler error by wrapping malloc
      1.44  (2014-08-07)
              various warning fixes from Ronny Chevalier
      1.43  (2014-07-15)
              fix MSVC-only compiler problem in code changed in 1.42
      1.42  (2014-07-09)
              don't define _CRT_SECURE_NO_WARNINGS (affects user code)
              fixes to stbi__cleanup_jpeg path
              added STBI_ASSERT to avoid requiring assert.h
      1.41  (2014-06-25)
              fix search&replace from 1.36 that messed up comments/error messages
      1.40  (2014-06-22)
              fix gcc struct-initialization warning
      1.39  (2014-06-15)
              fix to TGA optimization when req_comp != number of components in TGA;
              fix to GIF loading because BMP wasn't rewinding (whoops, no GIFs in my test suite)
              add support for BMP version 5 (more ignored fields)
      1.38  (2014-06-06)
              suppress MSVC warnings on integer casts truncating values
              fix accidental rename of 'skip' field of I/O
      1.37  (2014-06-04)
              remove duplicate typedef
      1.36  (2014-06-03)
              convert to header file single-file library
              if de-iphone isn't set, load iphone images color-swapped instead of returning NULL
      1.35  (2014-05-27)
              various warnings
              fix broken STBI_SIMD path
              fix bug where stbi_load_from_file no longer left file pointer in correct place
              fix broken non-easy path for 32-bit BMP (possibly never used)
              TGA optimization by Arseny Kapoulkine
      1.34  (unknown)
              use STBI_NOTUSED in stbi__resample_row_generic(), fix one more leak in tga failure case
      1.33  (2011-07-14)
              make stbi_is_hdr work in STBI_NO_HDR (as specified), minor compiler-friendly improvements
      1.32  (2011-07-13)
              support for "info" function for all supported filetypes (SpartanJ)
      1.31  (2011-06-20)
              a few more leak fixes, bug in PNG handling (SpartanJ)
      1.30  (2011-06-11)
              added ability to load files via callbacks to accomidate custom input streams (Ben Wenger)
              removed deprecated format-specific test/load functions
              removed support for installable file formats (stbi_loader) -- would have been broken for IO callbacks anyway
              error cases in bmp and tga give messages and don't leak (Raymond Barbiero, grisha)
              fix inefficiency in decoding 32-bit BMP (David Woo)
      1.29  (2010-08-16)
              various warning fixes from Aurelien Pocheville
      1.28  (2010-08-01)
              fix bug in GIF palette transparency (SpartanJ)
      1.27  (2010-08-01)
              cast-to-stbi_uc to fix warnings
      1.26  (2010-07-24)
              fix bug in file buffering for PNG reported by SpartanJ
      1.25  (2010-07-17)
              refix trans_data warning (Won Chun)
      1.24  (2010-07-12)
              perf improvements reading from files on platforms with lock-heavy fgetc()
              minor perf improvements for jpeg
              deprecated type-specific functions so we'll get feedback if they're needed
              attempt to fix trans_data warning (Won Chun)
      1.23    fixed bug in iPhone support
      1.22  (2010-07-10)
              removed image *writing* support
              stbi_info support from Jetro Lauha
              GIF support from Jean-Marc Lienher
              iPhone PNG-extensions from James Brown
              warning-fixes from Nicolas Schulz and Janez Zemva (i.stbi__err. Janez (U+017D)emva)
      1.21    fix use of 'stbi_uc' in header (reported by jon blow)
      1.20    added support for Softimage PIC, by Tom Seddon
      1.19    bug in interlaced PNG corruption check (found by ryg)
      1.18  (2008-08-02)
              fix a threading bug (local mutable static)
      1.17    support interlaced PNG
      1.16    major bugfix - stbi__convert_format converted one too many pixels
      1.15    initialize some fields for thread safety
      1.14    fix threadsafe conversion bug
              header-file-only version (#define STBI_HEADER_FILE_ONLY before including)
      1.13    threadsafe
      1.12    const qualifiers in the API
      1.11    Support installable IDCT, colorspace conversion routines
      1.10    Fixes for 64-bit (don't use "unsigned long")
              optimized upsampling by Fabian "ryg" Giesen
      1.09    Fix format-conversion for PSD code (bad global variables!)
      1.08    Thatcher Ulrich's PSD code integrated by Nicolas Schulz
      1.07    attempt to fix C++ warning/errors again
      1.06    attempt to fix C++ warning/errors again
      1.05    fix TGA loading to return correct *comp and use good luminance calc
      1.04    default float alpha is 1, not 255; use 'void *' for stbi_image_free
      1.03    bugfixes to STBI_NO_STDIO, STBI_NO_HDR
      1.02    support for (subset of) HDR files, float interface for preferred access to them
      1.01    fix bug: possible bug in handling right-side up bmps... not sure
              fix bug: the stbi__bmp_load() and stbi__tga_load() functions didn't work at all
      1.00    interface to zlib that skips zlib header
      0.99    correct handling of alpha in palette
      0.98    TGA loader by lonesock; dynamically add loaders (untested)
      0.97    jpeg errors on too large a file; also catch another malloc failure
      0.96    fix detection of invalid v value - particleman@mollyrocket forum
      0.95    during header scan, seek to markers in case of padding
      0.94    STBI_NO_STDIO to disable stdio usage; rename all #defines the same
      0.93    handle jpegtran output; verbose errors
      0.92    read 4,8,16,24,32-bit BMP files of several formats
      0.91    output 24-bit Windows 3.0 BMP files
      0.90    fix a few more warnings; bump version number to approach 1.0
      0.61    bugfixes due to Marc LeBlanc, Christopher Lloyd
      0.60    fix compiling as c++
      0.59    fix warnings: merge Dave Moore's -Wall fixes
      0.58    fix bug: zlib uncompressed mode len/nlen was wrong endian
      0.57    fix bug: jpg last huffman symbol before marker was >9 bits but less than 16 available
      0.56    fix bug: zlib uncompressed mode len vs. nlen
      0.55    fix bug: restart_interval not initialized to 0
      0.54    allow NULL for 'int *comp'
      0.53    fix bug in png 3->4; speedup png decoding
      0.52    png handles req_comp=3,4 directly; minor cleanup; jpeg comments
      0.51    obey req_comp requests, 1-component jpegs return as 1-component,
              on 'test' only check type, not whether we support this variant
      0.50  (2006-11-19)
              first released version
*/


/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/