#include <nghttp2/nghttp2.h>

#define DEFAULT_MAX_BUFLEN 4096

void hpackQ___ext_init__() {
}

struct hpackQ_DeflaterG_class;

struct hpackQ_Deflater {
    struct hpackQ_DeflaterG_class *$class;
    nghttp2_hd_deflater *deflater;
};

B_NoneType hpackQ_DeflaterD___init__(hpackQ_Deflater self) {
    nghttp2_hd_deflate_new(&self->deflater, DEFAULT_MAX_BUFLEN);
    return B_None;
}

B_NoneType hpackQ_DeflaterD___del__(hpackQ_Deflater self) {
    nghttp2_hd_deflate_del(self->deflater);
    return B_None;
}

B_bytes hpackQ_DeflaterD_deflate(hpackQ_Deflater self, B_dict headers) {
    return to$bytes("");
}


struct hpackQ_InflaterG_class;

struct hpackQ_Inflater {
    struct hpackQ_InflaterG_class *$class;
    nghttp2_hd_inflater *inflater;
};

B_NoneType hpackQ_InflaterD___init__(hpackQ_Inflater self) {
    nghttp2_hd_inflate_new(&self->inflater);
    return B_None;
}

B_NoneType hpackQ_InflaterD___del__(hpackQ_Inflater self) {
    nghttp2_hd_inflate_del(self->inflater);
    return B_None;
}

B_dict hpackQ_inflate(B_bytes compressed_headers) {
    B_dict ret;
    return ret;
}
