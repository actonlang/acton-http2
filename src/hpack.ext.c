#include <nghttp2/nghttp2.h>

#define DEFAULT_MAX_BUFLEN 4096

void hpackQ___ext_init__() {
}

B_bytes hpackQ_deflate(B_dict headers) {
    nghttp2_hd_deflater *deflater;
    nghttp2_hd_deflate_new(&deflater, DEFAULT_MAX_BUFLEN);
    nghttp2_hd_deflate_del(deflater);
    return to$bytes("");
}

B_dict hpackQ_inflate(B_bytes compressed_headers) {
    B_dict ret;
    nghttp2_hd_inflater *inflater;
    nghttp2_hd_inflate_new(&inflater);
    nghttp2_hd_inflate_del(inflater);
    return ret;
}
